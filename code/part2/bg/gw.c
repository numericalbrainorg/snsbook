#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#define DT 1. // ms

#define RAND_SEED ((unsigned) time(NULL))
//#define RAND_SEED 10

// layer constants
#define N_STATES_X 11
#define N_STATES_Y 11
#define N_STATES   (N_STATES_X*N_STATES_Y)
#define N_CRITICS  100
#define N_ACTORS   180

// neuron constants
#define AHP        -5.   // mV       : after hyperpolarization amplitude
#define TAU_M      20.   // ms       : membrane and post synaptic potential decay time constant
#define TAU_S       5.   // ms       : post synaptic potential rise time constant
#define EPSILON_0  20.   // mV*ms    : post synaptic potential scalling constant
#define RHO_0       0.06 // kHz      : poisson firing scalling constant
#define RHO_PC      0.4  // kHz      : poisson firing scalling constant for state spike
#define THRESHOLD  16.   // mV       : threshold
#define DELTA_U     2.   // mV       : threshold response scalling constant
#define DELTA_PC    2.   // [length] : length response scalling constant for state spike

// synaptic weight constants
#define W_FLOOR         0.5  // weight floor for learning
#define W_CEIL          3.   // weight ceil for learning
#define W_PLUS         30.   // actor lateral connection constant
#define W_MINUS       -60.   // actor lateral connection constant
#define W_INIT_CRITIC   0.77 // average initial weight for critic
#define W_INIT_ACTOR    0.77 // average initial weight for actor
#define XI              8    // actor lateral connection constant

// td-learning constants
#define ALPHA_CRITIC  0.02  // critic learning rate
#define ALPHA_ACTOR   0.005 // actor  learning rate
#define NU         2000.    // ms
#define V_0         -40.    // [reward]
#define TAU_R      4000.    // ms

#define TAU_KAPPA1  200. // ms
#define TAU_KAPPA2   50. // ms
#define TAU_GAMMA1   50. // ms
#define TAU_GAMMA2   20. // ms
#define TAU_REWARD1 200. // ms
#define TAU_REWARD2  10. // ms

// task setting constants
#define MAX_TRIAL   100
#define T_TIMEOUT 50000. // msec : time limit per trial
#define T_CLAMP    1000. // msec : non-learning waiting time for steady state
#define T_NEUTRAL  1000. // msec : learning waiting time after reaching goal
#define R_OBST       -1. // [reward]
#define R_GOAL      100. // [reward]
#define GOAL_X        4  // [length]
#define GOAL_Y        0  // [length]
#define START_X      -4  // [length]
#define START_Y       0  // [length]

// data output setting
#define OUTPUT_INTERVAL_STEP 100

// random functions
double uniform()
{
    return ((double)rand() + 1.) / ((double)RAND_MAX + 2.);
}

double rand_normal(double mu, double sigma)
{
    return mu + sigma * sqrt(-2. * log(uniform())) * sin(2. * M_PI * uniform());
}

// spiking neuron layer
typedef struct
{
    int n;
    int* spike;
    double* u;   // menbrane potential
    double* ahp; // after hyper polarization
    double* psp; // post synaptic potential
} Layer;

void reset_layer(Layer layer)
{
    for(int i = 0; i < layer.n; i++)
    {
        layer.spike[i] = 0;
        layer.u[i]     = 0.;
        layer.ahp[i]   = 0.;
        layer.psp[i]   = 0.;
    }
}

Layer malloc_layer(int n)
{
    Layer layer;
    layer.n     = n;
    layer.spike = (int*)malloc(sizeof(int) * n);
    layer.u     = (double*)malloc(sizeof(double) * n);
    layer.ahp   = (double*)malloc(sizeof(double) * n);
    layer.psp   = (double*)malloc(sizeof(double) * n);
    return layer;
}

void free_layer(Layer layer)
{
    free(layer.spike);
    free(layer.u);
    free(layer.ahp);
    free(layer.psp);
}

void update_layer(Layer layer)
{
    double ahp_decay = exp(-DT / TAU_M);
    for(int i = 0; i < layer.n; ++i)
    {
        layer.u[i]     = layer.psp[i] + AHP * layer.ahp[i];
        layer.spike[i] = (uniform() < DT * RHO_0 * exp((layer.u[i] - THRESHOLD) / DELTA_U)) ? 1 : 0;
        layer.ahp[i]   = (layer.spike[i] == 1) ? 1 : layer.ahp[i] * ahp_decay;
        layer.psp[i] = 0.;
    }
}

void set_state_spike(int* state_spike, double state_x, double state_y)
{
    for(int i = 0; i < N_STATES_X; i++)
    {
        for(int j = 0; j < N_STATES_Y; j++)
        {
            double dx = state_x - DELTA_PC * (i - N_STATES_X / 2);
            double dy = state_y - DELTA_PC * (j - N_STATES_Y / 2);
            state_spike[i * N_STATES_Y + j] = uniform() < DT * RHO_PC * exp(- (dx * dx + dy * dy) / (DELTA_PC * DELTA_PC)) ? 1 : 0;
        }
    }
}

// exponential kernel

typedef struct
{
    int n;
    double *v;  // e1 - e2
    double *e1; // exp(-s / tau_decay) * Heaviside(s)
    double *e2; // exp(-s / tau_rise)  * Heaviside(s)
} Kernel;

void reset_kernel(Kernel kernel)
{
    for(int i = 0; i < kernel.n; i++)
    {
        kernel.v[i] = kernel.e1[i] = kernel.e2[i] = 0.;
    }
}

Kernel malloc_kernel(int n)
{
    Kernel kernel;
    kernel.n  = n;
    kernel.v  = (double*)malloc(sizeof(double) * n);
    kernel.e1 = (double*)malloc(sizeof(double) * n);
    kernel.e2 = (double*)malloc(sizeof(double) * n);
    return kernel;
}

void free_kernel(Kernel kernel)
{
    free(kernel.v);
    free(kernel.e1);
    free(kernel.e2);
}


// synapse functions

void init_weight_normal(int n, double w[n], double mu, double sigma)
{
    for(int i = 0; i < n; i++)
    {
        w[i] = rand_normal(mu, sigma);
    }
}

void update_epsilon(int n_pre, int n_post, Kernel epsilon, int spike_pre[n_pre], int spike_post[n_post])
{
    double d1 = exp(-DT / TAU_M);
    double d2 = exp(-DT / TAU_S);
    double a  = EPSILON_0 / (TAU_M - TAU_S);
    for(int i = 0; i < n_post; i++)
    {
        if(spike_post[i] == 1)
        {
            for(int j = 0; j < n_pre; j++)
            {
                int k = i * n_pre + j;
                epsilon.v[k] = epsilon.e1[k] = epsilon.e2[k] = 0.;
            }
        }
        else
        {
            for(int j = 0; j < n_pre; j++)
            {
                int k = i * n_pre + j;
                epsilon.e1[k] = epsilon.e1[k] * d1 + spike_pre[j];
                epsilon.e2[k] = epsilon.e2[k] * d2 + spike_pre[j];
                epsilon.v[k]  = a * (epsilon.e1[k] - epsilon.e2[k]);
            }
        }
    }
}

void weight_epsilon_sum(int n_pre, int n_post, double wepsilon[n_post], double weight[n_post * n_pre], Kernel epsilon)
{
    for(int i = 0; i < n_post; i++)
    {
        for(int j = 0; j < n_pre; j++)
        {
            int k = i * n_pre + j;
            wepsilon[i] += weight[k] * epsilon.v[k];
        }
    }
}

// TD-LTP window
void update_kappa(int n_pre, int n_post, Kernel kappa, Kernel epsilon, int spike_post[n_post])
{
    double d1 = exp(-DT / TAU_KAPPA1);
    double d2 = exp(-DT / TAU_KAPPA2);
    double a  = 1. / (TAU_KAPPA1 - TAU_KAPPA2);
    for(int i = 0; i < n_post; i++)
    {
        for(int j = 0; j < n_pre; j++)
        {
            int k = i * n_pre + j;
            kappa.e1[k] = kappa.e1[k] * d1 + epsilon.v[k] * spike_post[i];
            kappa.e2[k] = kappa.e2[k] * d2 + epsilon.v[k] * spike_post[i];
            kappa.v[k]  = a * (kappa.e1[k] - kappa.e2[k]);
        }
    }
}

void update_kappa_value(Kernel kappa, Kernel dkappa, Layer critic)
{
    double d1 = exp(-DT / TAU_KAPPA1);
    double d2 = exp(-DT / TAU_KAPPA2);
    double a  = 1. / (TAU_KAPPA1 - TAU_KAPPA2);
    for(int i = 0; i < N_CRITICS; i++)
    {
        kappa.e1[i] = kappa.e1[i] * d1 + critic.spike[i];
        kappa.e2[i] = kappa.e2[i] * d2 + critic.spike[i];
        kappa.v[i]  = a * (kappa.e1[i] - kappa.e2[i]);
        dkappa.v[i]  = a * (- kappa.e1[i] / TAU_KAPPA1 + kappa.e2[i] / TAU_KAPPA2);
    }
}

double sum_kernel(Kernel kernel)
{
    double sum = 0.;
    for(int i = 0; i < kernel.n; i++)
    {
        sum += kernel.v[i];
    }
    return sum;
}

double get_tderror(double dvalue, double value, double reward)
{
    //return (NU / N_CRITICS) * (dvalue - value / TAU_R) - V_0 / TAU_R + reward;
    return dvalue - value / TAU_R + reward; 
}

void update_weight(int n, double weight[n], Kernel kappa, double alpha, double tderror)
{
    for(int i = 0; i < n; i++)
    {
        weight[i] += DT * alpha * tderror * kappa.v[i];
        weight[i]  = weight[i] > W_CEIL  ? W_CEIL  : weight[i];
        weight[i]  = weight[i] < W_FLOOR ? W_FLOOR : weight[i];
    }
}

// action functions

void update_gamma(Kernel gamma, Layer actor)
{
    double d1 = exp(-DT / TAU_GAMMA1);
    double d2 = exp(-DT / TAU_GAMMA2);
    double a  = 1. / (TAU_GAMMA1 - TAU_GAMMA2);
    for(int i = 0; i < actor.n; i++)
    {
        gamma.e1[i] = gamma.e1[i] * d1 + actor.spike[i];
        gamma.e2[i] = gamma.e2[i] * d2 + actor.spike[i];
        gamma.v[i]  = a * (gamma.e1[i] - gamma.e2[i]);
    }
}

void get_action(double *action_x, double *action_y, Kernel gamma)
{
    *action_x = 0.;
    *action_y = 0.;
    for(int i = 0; i < N_ACTORS; i++)
    {
        double theta = 2. * i * M_PI / N_ACTORS;
        *action_x += cos(theta) * gamma.v[i];
        *action_y += sin(theta) * gamma.v[i];
    }
    *action_x /= N_ACTORS;
    *action_y /= N_ACTORS;
}

int move_maze(double *state_x, double *state_y, double action_x, double action_y)
{
    // new position
    double new_x = *state_x + action_x * DT;
    double new_y = *state_y + action_y * DT;

    // out of area
    double bound_x = DELTA_PC * ((N_STATES_X - 2) / 2);
    double bound_y = DELTA_PC * ((N_STATES_Y - 2) / 2);
    if(new_x <= -bound_x) new_x = -bound_x + 0.1;
    if(new_x >=  bound_x) new_x =  bound_x - 0.1;
    if(new_y <= -bound_y) new_y = -bound_y + 0.1;
    if(new_y >=  bound_y) new_y =  bound_y - 0.1;
    
    int flag_collision = 0;
    if((new_x != *state_x + action_x) || (new_y != *state_y + action_y)) flag_collision = 1;
    
    *state_x = new_x;
    *state_y = new_y;
    
    return flag_collision;
}


void print_raster(FILE* fp, double t, int offset, int n, int spike[n])
{
    for(int i = 0; i < n; i++)
    {
        if(spike[i] == 1)
        {
            fprintf(fp, "%f %d\n", t, i + offset);
        }        
    }
}


int main()
{
    srand(RAND_SEED);
    FILE* fp_raster = fopen("raster.dat","w");
    FILE* fp_td     = fopen("td.dat","w");
    FILE* fp_pos    = fopen("pos.dat","w");

    // state spike
    int state_spike[N_STATES];

    // spiking neuron layer
    Layer actor  = malloc_layer(N_ACTORS);
    Layer critic = malloc_layer(N_CRITICS);

    // synaptic weights : w_ij = w[i * n_pre + j]
    // i : post synaptic neuron id
    // j : pre  synaptic neuron id
    double weight_state2actor[N_ACTORS * N_STATES];
    double weight_state2critic[N_CRITICS * N_STATES];
    double weight_actor2actor[N_ACTORS * N_ACTORS];

    // post synaptic potential : convolution(X_i, epsilon)(t)
    Kernel epsilon_state2actor  = malloc_kernel(N_ACTORS * N_STATES);
    Kernel epsilon_state2critic = malloc_kernel(N_CRITICS * N_STATES);
    Kernel epsilon_actor2actor  = malloc_kernel(N_ACTORS * N_ACTORS);
    
    // TD-LTP window : convolution[convolution(X_i, epsilon),kappa](t)
    Kernel kappa_state2actor  = malloc_kernel(N_STATES * N_ACTORS);
    Kernel kappa_state2critic = malloc_kernel(N_STATES * N_CRITICS);

    // value function : convolution(X_i^critic, kappa)(t), convolution(X_i^critic, \dot{kappa})
    Kernel kappa_value = malloc_kernel(N_CRITICS);
    Kernel dkappa_value = malloc_kernel(N_CRITICS);

    // action kernel : convolution(X_i^actor, gamma)
    Kernel gamma = malloc_kernel(N_ACTORS);

    // initialize weights from state to actor with normal distribution
    init_weight_normal(N_STATES * N_ACTORS,  weight_state2actor,  W_INIT_ACTOR,  0.1);
    init_weight_normal(N_STATES * N_CRITICS, weight_state2critic, W_INIT_CRITIC, 0.1);

    // initialize weights from actor to actor
    double zfk = 0.;
    for(int k = 1; k < N_ACTORS; k++)
    {
        double theta_k = 2. * k * M_PI / N_ACTORS;
        zfk += exp(XI * cos(theta_k));
    }
    
    for(int i = 0; i < N_ACTORS; i++)
    {
        double theta_i = 2. * i * M_PI / N_ACTORS;
        for(int j = 0; j < N_ACTORS; j++)
        {
            double theta_j = 2. * j * M_PI / N_ACTORS;
            double f_ij    = (i != j) ? exp(XI * cos(theta_i - theta_j)) : 0.;
            weight_actor2actor[i * N_ACTORS + j] = W_MINUS / N_ACTORS + W_PLUS * f_ij / zfk;
        }
    }
    

    for(int trial = 0; trial < MAX_TRIAL; trial++)
    {
        fprintf(stderr, "trial = %d\n", trial);

        int    max_step  = T_TIMEOUT / DT;
        double state_x   = START_X;
        double state_y   = START_Y;
        double action_x  = 0.;
        double action_y  = 0.;
        double reward    = 0.;
        double reward_e1 = 0.;
        double reward_e2 = 0.;
        double value     = 0.;
        double dvalue    = 0.;
        double tderror   = 0.;
        int    flag_end  = 0;

        reset_layer(actor);
        reset_layer(critic);

        reset_kernel(epsilon_actor2actor);
        reset_kernel(epsilon_state2actor);
        reset_kernel(epsilon_state2critic);
        reset_kernel(kappa_state2actor);
        reset_kernel(kappa_state2critic);
        reset_kernel(kappa_value);
        reset_kernel(dkappa_value);
        reset_kernel(gamma);

        for(int step = 0; step < max_step; step++)
        {
            double t = step * DT;
            if(step % OUTPUT_INTERVAL_STEP == 0)
            {
                fprintf(fp_pos, "%f %f %f\n", t, state_x, state_y);
                fprintf(fp_td,  "%f %f %f\n", t, value, tderror);
            }
            print_raster(fp_raster, t, 0,   N_STATES,  state_spike);
            print_raster(fp_raster, t, 200, N_ACTORS,  actor.spike);
            print_raster(fp_raster, t, 400, N_CRITICS, critic.spike);

            // update state neuron spike
            if(flag_end == 0)
            {
                set_state_spike(state_spike, state_x, state_y);
            }

            // get action from gamma kernel
            get_action(&action_x, &action_y, gamma);
            update_gamma(gamma, actor);
            
            // move maze by action
            double r = 0.;
            if(flag_end == 0 && t > T_CLAMP)
            {
                int collision = move_maze(&state_x, &state_y, action_x, action_y);
                if(collision == 1)
                {
                    r = -1;
                }

                // goal
                double dx = state_x - GOAL_X;
                double dy = state_y - GOAL_Y;
                if(sqrt(dx * dx + dy * dy) < 1)
                {
                    r = 100;
                    flag_end = 1;
                    max_step = step + 1000. / DT;
                }
                // turn off state neurons
                if(flag_end == 1)
                {
                    for(int i = 0; i < N_STATES; i++) state_spike[i] = 0;
                }
            }

            // update actor and critic layer
            update_layer(actor);
            update_layer(critic);

            // reward kernel
            reward_e1 += DT * -reward_e1 / TAU_REWARD1 + r;
            reward_e2 += DT * -reward_e2 / TAU_REWARD2 + r;
            reward     = (reward_e1 - reward_e2) / (TAU_REWARD1 - TAU_REWARD2);

            // compute td-error
            if(flag_end == 0)
            {
                value  = (NU / N_CRITICS) * sum_kernel(kappa_value) - V_0;
                dvalue = (NU / N_CRITICS) * sum_kernel(dkappa_value);
            }
            else
            {
                // exponential decaying at trial end
                value *= exp(-DT / TAU_KAPPA1);
                dvalue = -value / TAU_KAPPA1;
            }
            tderror = get_tderror(dvalue, value, reward);

            // update value function kernel
            update_kappa_value(kappa_value, dkappa_value, critic);
            
            // synaptic weight chainge
            if(t > T_CLAMP)
            {
                update_weight(N_STATES * N_ACTORS,  weight_state2actor,  kappa_state2actor,  ALPHA_ACTOR,  tderror);
                update_weight(N_STATES * N_CRITICS, weight_state2critic, kappa_state2critic, ALPHA_CRITIC, tderror);
            }
            
            // kappa kernel (stdp window)
            update_kappa(N_STATES, N_ACTORS, kappa_state2actor, epsilon_state2actor, actor.spike);
            update_kappa(N_STATES, N_CRITICS, kappa_state2critic, epsilon_state2critic, critic.spike);

            // epsilon kernel (post synaptic potential)
            update_epsilon(N_STATES, N_ACTORS,  epsilon_state2actor,  state_spike, actor.spike);
            update_epsilon(N_STATES, N_CRITICS, epsilon_state2critic, state_spike, critic.spike);
            update_epsilon(N_ACTORS, N_ACTORS,  epsilon_actor2actor,  actor.spike, actor.spike);

            // weight epsilon product (post synaptic potential)
            weight_epsilon_sum(N_STATES, N_ACTORS,  actor.psp,  weight_state2actor,  epsilon_state2actor);
            weight_epsilon_sum(N_STATES, N_CRITICS, critic.psp, weight_state2critic, epsilon_state2critic);
            weight_epsilon_sum(N_ACTORS, N_ACTORS,  actor.psp,  weight_actor2actor,  epsilon_actor2actor);
        }
        fprintf(fp_raster, "\n\n");
        fprintf(fp_td,     "\n\n");
        fprintf(fp_pos,    "\n\n");
    }
}
