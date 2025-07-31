#### Instruction of the branch predictor
In the realization of the Tosumalo CPU supporting RISCV-32I ISA, we use the 
Tournament predictor as its branch predictor, which includes two 2-level predictor based on global history and local history respectively and a meta 
predictor selecting on which one a certain instruction to rely.

#### Performance 
The performance of the branch predictor is evaluated by the error rate, more specifically, times when the predict of whether it should jump contradicts to the result / the number of branches faced by the program.  
The result is as follows:   
|   Test name    | Error rate |
| :------------: | :--------: |
|  array_test1   |    0.3     |
|  array_test2   |  0.709091  |
|   basicopt1    |  0.111597  |
|   bulgarian    | 0.0726744  |
|      expr      |  0.398607  |
|      gcd       |  0.491979  |
|     hanoi      |  0.14641   |
|    lvalue2     |  0.33333   |
|     magic      |  0.14865   |
| manyarguments  |    0.55    |
|   multiarray   |  0.33125   |
|       pi       |    ???     |
|     qsort      | 0.0792181  |
|     queens     |  0.214196  |
| statement_test |  0.459016  |
|   superloop    |  0.045169  |
|      tak       | 0.0650684  |