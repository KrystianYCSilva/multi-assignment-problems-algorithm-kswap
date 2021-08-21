/*
SOLUTION OF THE LINEAR MIN-SUM ASSIGNMENT PROBLEM. 
 */

#ifndef APC_H
#define APC_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        int num_cols;
        int** cost;
        int cost_optmal_assignment;
        int number_solution;
        int* colunm_assignment;
        int cost_solution;
    } apc_problem_t;

    int apc_init(apc_problem_t* p, int n, int** a, int z_p, int s);
    void apc_free(apc_problem_t* p);
    /*
       
     SOLUTION OF THE LINEAR MIN-SUM ASSIGNMENT PROBLEM.                         

     MEANING OF THE INPUT PARAMETERS:          
     n       = NUMBER OF ROWS AND COLUMNS OF THE COST MATRIX.     
     a[i][j] = COST OF THE ASSIGNMENT OF ROW  i  TO COLUMN  j .   
     INF     = A VERY LARGE INTEGER VALUE, SETTED BY THE USER ACCORDING TO THE CHARACTERISTICS OF THE USED MACHINE.
                       INF IS THE ONLY MACHINE-DEPENDENT CONSTANT USED AND IT MUST BE STRICTLY GREATER THAN THE MAXIMUM 
                       ASSIGNMENT COST (E.G., INF MUST BE STRICTLY GREATER THAN THE MAXIMUM VALUE OF THE COST MATRIX a).
     ON RETURN, THE INPUT PARAMETERS ARE UNCHANGED.              
       
     MEANING OF THE OUTPUT PARAMETERS:       
     f[i] = COLUMN ASSIGNED TO ROW i.        
     z_p  = COST OF THE OPTIMAL ASSIGNMENT =   
          = a[0][f[0]] + a[1][f[1]] + ... + a[n-1][f[n-1]] .  
	  
     RETURN VALUE:
     0,  IF THE PROBLEM HAS BEEN SOLVED AND THE OUTPUT PARAMETERS HAVE BEEN PROPERLY SETTED
     -1, IF THE PROBLEM HAS NOT BEEN SOLVED DUE TO A MEMORY ISSUE (NOT ENOUGH AVAILABLE MEMORY)
       
     ALL THE PARAMETERS ARE INTEGERS.
     VECTOR  f  MUST BE DIMENSIONED AT LEAST AT  n , MATRIX  a
     AT LEAST AT  (n,n) . 
       
     */
    /***
     * Calculates de minimun assignment problem
     * @param n Numeber of rows and columns
     * @param a Cost matrix
     * @param INF Max integer
     * @param z_p Cost of the optimal assignment
     * @param f Column assigned to row i
     * @param s Number of solutions found (placeholder)
     * @return -1 if there there were problems, else 0
     */
    int apc(apc_problem_t* p);



#ifdef __cplusplus
}
#endif


#endif