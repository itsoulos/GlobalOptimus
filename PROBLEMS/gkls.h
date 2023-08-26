#ifndef GKLS_H
#define GKLS_H
#define KK 100                     /* the long lag */
#define LL  37                     /* the short lag */
#define mod_sum(x,y) (((x)+(y))-(int)((x)+(y)))   /* (x+y) mod 1.0 */

#define TT  70   /* guaranteed separation between streams */
#define is_odd(s) ((s)&1)

#define QUALITY 1009 /* recommended quality level for high-res use */

#define NUM_RND 1009 /* size of the array of random numbers */


extern double rnd_num[NUM_RND]; /* array of random numbers */

/* For rnd_gen.c */
void ranf_array(double aa[], int n); /* put n new random fractions in aa */
  /* double *aa  - destination */
  /* int n       - array length (must be at least KK) */
void ranf_start(long seed);  /* do this before using ranf_array */
  /* long seed   - selector for different streams */

/****************************************************************************/
/*                                                                          */
/*    Copyright (c) 2000 by Donald E. Knuth                                 */
/*    It is explained in Seminumerical Algorithms, 3rd edition, Section 3.6 */
/*    (or in the errata to the 2nd edition --- see                          */
/*        http://www-cs-faculty.stanford.edu/~knuth/taocp.html              */
/*    in the changes to pages 171 and following of Volume 2).               */
/*                                                                          */
/*  The functions of this file implement a random number generator.         */
/*  There are two functions: ranf_array() and ranf_start().                 */
/*  The logical-and operation '&' is used  for efficiency, so this          */
/*  random generator is not strictly portable unless the computer uses      */
/*  two's complement representation for integer. It does not limit          */
/*  application of the package because almost all modern computers are      */
/*  based on two's complement arithmetic.                                   */
/*                                                                          */
/*    The function ranf_start (long} seed)                                  */
/*  initializes the generator when given a seed number between              */
/*  0 and 2^30 = 1,073,741,821.                                             */
/*    The function ranf_array (double aa[], int n) generates n new random   */
/*  numbers and places them into a given array aa, using the recurrence     */
/*    X(j) = [X(j-100) - X(j-37)] mod 2^30,                                 */
/*  that is particularly well studied to modern computers. The value n must */
/*  be at least 100; larger values like 1000 are recommended, by default,   */
/*    n = NUM_RND = 1009.                                                   */
/*  (see also the header file rnd_gen.h)                                    */
/*                                                                          */
/*********** See the book of D.Knuth for explanations and caveats! **********/


/* Penalty value of the generated function if x is not in D */
#define GKLS_MAX_VALUE        1E+100

/* Value of the machine zero in the floating-point arithmetic */
#define GKLS_PRECISION        1.0E-10

/* Default value of the paraboloid minimum */
#define GKLS_PARABOLOID_MIN   0.0

/* Global minimum value: to be less than GKLS_PARABOLOID_MIN */
#define GKLS_GLOBAL_MIN_VALUE -1.0

/* Max value of the parameter delta for the D2-type class function        */
/* The parameter delta is chosen randomly from (0, GKLS_DELTA_MAX_VALUE ) */
#define GKLS_DELTA_MAX_VALUE  10.0

/* Constant pi */
#ifndef PI
#define PI 3.14159265
#endif

/* Error codes */
#define GKLS_OK                              0
#define GKLS_DIM_ERROR                       1
#define GKLS_NUM_MINIMA_ERROR                2
#define GKLS_FUNC_NUMBER_ERROR               3
#define GKLS_BOUNDARY_ERROR                  4
#define GKLS_GLOBAL_MIN_VALUE_ERROR          5
#define GKLS_GLOBAL_DIST_ERROR               6
#define GKLS_GLOBAL_RADIUS_ERROR             7
#define GKLS_MEMORY_ERROR                    8
#define GKLS_DERIV_EVAL_ERROR                9

/* Reserved error codes */
#define GKLS_GREAT_DIM                      10
#define GKLS_RHO_ERROR                      11
#define GKLS_PEAK_ERROR                     12
#define GKLS_GLOBAL_BASIN_INTERSECTION      13

/* Internal error codes */
#define GKLS_PARABOLA_MIN_COINCIDENCE_ERROR 14
#define GKLS_LOCAL_MIN_COINCIDENCE_ERROR    15
#define GKLS_FLOATING_POINT_ERROR           16



/* The next two structures define a list of all local minima and    */
/* a list of global minima.                                         */
/* These lists are filled by the generator.                         */
/* The fields of the structures help to the user                    */
/* to study properties of a concrete generated test function        */

/* The structure of type T_GKLS_Minima contains the following          */
/* information about all local minima (including the paraboloid        */
/* minimum and the global one): coordinates of local minimizers,       */
/* local minima values, and attraction regions radii.                  */
typedef struct {
         double **local_min; /* list of local minimizers coordinates   */
         double *f;          /* list of local minima values            */
         double *w_rho;      /* list of radius weights                 */
         double *peak;       /* list of parameters gamma(i) =          */
                             /*  = local minimum value(i) - paraboloid */
                             /*    minimum within attraction regions   */
                             /*    of local minimizer(i)               */
         double *rho;        /* list of attraction regions radii       */
} T_GKLS_Minima;

/* The structure of type T_GKLS_GlobalMinima contains information      */
/* about the number of global minimizers and their                     */
/* indexes in the set of local minimizers                              */

typedef struct {
         unsigned int num_global_minima; /* number of global minima    */
         unsigned int *gm_index;  /* list of indexes of generated      */
         /* minimizers which are the global ones (the element from 0   */
         /* to (num_global_minima - 1) of the list) and the local ones */
         /* (the resting elements of the list)                         */
} T_GKLS_GlobalMinima;


/*-------------- Variables accessible by the user --------------------- */
extern double *GKLS_domain_left; /* left boundary vector of D  */
  /* D=[GKLS_domain_left; GKLS_domain_ight] */
extern double *GKLS_domain_right;/* right boundary vector of D */

extern unsigned int GKLS_dim;    /* dimension of the problem,        */
                                 /* 2<=test_dim<NUM_RND (see random) */
extern unsigned int GKLS_num_minima; /* number of local minima, >=2  */

extern double GKLS_global_dist;  /* distance from the paraboloid minimizer  */
                                 /* to the global minimizer                 */
extern double GKLS_global_radius;/* radius of the global minimizer          */
                                 /* attraction region                       */
extern double GKLS_global_value; /* global minimum value,                   */
                                 /* test_global_value < GKLS_PARABOLOID_MIN */
extern T_GKLS_Minima GKLS_minima;
                                 /* see the structures type description     */
extern T_GKLS_GlobalMinima GKLS_glob;


/*------------------------User function prototypes -------------------------*/

int GKLS_domain_alloc(void); /* allocate boundary vectors   */

void GKLS_domain_free(void); /* deallocate boundary vectors */

int  GKLS_set_default(void); /* set default values of the input parameters  */
                          /* and allocate the boundary vectors if necessary */
void GKLS_free(void);        /* deallocate memory needed for the generator  */

int GKLS_parameters_check(void);/* test the validity of the input parameters*/

int GKLS_arg_generate (unsigned int); /* test function generator */

double GKLS_ND_func(double *);  /* evaluation of ND-typed test function   */

double GKLS_D_func(double *);   /* evaluation of D-typed test function    */

double GKLS_D2_func(double *);  /* evaluation of D2-type test function    */

double GKLS_D_deriv(unsigned int, double *);
             /* first order partial derivative of D-typed test function   */
double GKLS_D2_deriv1(unsigned int, double *);
             /* first order partial derivative of D2-typed test function  */
double GKLS_D2_deriv2(unsigned int, unsigned int, double *);
             /* second order partial derivative of D2-typed test function */
int GKLS_D_gradient  (double *, double *); /* gradient of D-type test function  */

int GKLS_D2_gradient (double *, double *); /* gradient of D2-type test function */

int GKLS_D2_hessian  (double *, double **);/* Hessian of D2-type test function  */


double GKLS_norm(double *, double *);
int GKLS_alloc(void);
int GKLS_coincidence_check(void);
int GKLS_set_basins(void);
int GKLS_initialize_rnd(unsigned int, unsigned int, int);


#endif // GKLS_H
