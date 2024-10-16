---
title: OPTIMUS: a multidimensional global optimization package
tags:
  - Global optimization
  - Evolutionary methods
  - Stopping rules
authors:
  - name: Ioannis G. Tsoulos
    equal-contrib: true
    affiliation: "1"
    corresponding: true
  - name: Vasileios Charilogis
    equal-contrib: true
    affiliation: "1"
  - name: Glykeria Kyrou
    equal-contrib: true
    affiliation: "1"
  - name: Vasileios N. Stavrou
    equal-contrib: true
    affiliation: "2"
  - name: Alexandros Tzallas
    equal-contrib: true
    affiliation: "1"
affiliations:
 - name: Department of Informatics and Telecommunications, University of Ioannina, Greece
   index: 1
 - name: Division of Physical Sciences, Hellenic Naval Academy, Greece
   index: 2
date: 16 October 2024
---

## Summary
The location of global minimum founds application in a variety of problems derived from physics, chemistry, medicine etc. Also, the global optimization process can be used in complex problems, such as the training of neural networks. Methods used in the global optimization process can be divided into stochastic and deterministic and this article proposes a software that incorporates a variety of stochastic methods.


# Statement of need
<p style="text-align: justify;">
This paper presents the `GlobalOptimus` software tool,  written in ANSI C++, which researchers can use to formulate the problem to be solved and then apply the required global optimization method identify the global minimum of the problem. The main features of the suggested software are: a) Coding of the objective problem in a high level language such as ANSI C++ b) Incorporation of many global optimization techniques to tackle the objective problem c)Parameterization of global optimization methods using user-defined parameters d) Usage of a GUI application to control the optimization strategy.
</p>

##  Software 
<p style="text-align: justify;">
The suggested software is entirely coded in ANSI C++, using  programming library available from https://qt.io. The researcher should code the objective function and a number of other mandatory functions in the C++ programming language. Also, the researcher should provide the dimension of the objective function as well as the bound of the function. Subsequently, the user can select a global optimization method to apply to the problem from a wide range of available methods.</p>

### Implemented global optimization methods 
<p style="text-align: justify?">
 In the proposed software, each implemented global optimization method has a set of parameters that can determine the overall optimization path and the effectiveness of the method. Some of the global optimization methods are as follows: </p>
 

METHOD |  DESCRIPTION |
|----------|----------|
| **Differential Evolution**   |has been widely used in many areas.|                                 
| **Parallel Differential Evolution**| divides the work into a number of available parallel computing units(V. Charilogis et al., 2023).|         
| **Double precision genetic algorithm**  | is included in the software and it is denoted as Genetic(I.G. Tsoulos et al., 2008).|                                    
| **Improved Particle Swarm Optimization**  |suggested by Charilogis and Tsoulos(V. Charilogis et al., 2022).|                       
| **Multistart**  |initiates local searches from different initial points is also implemented in the software.| 
| **NeuralMinimizer**| incorporates Radial Basis Functions (RBF) denoted by the name NeuralMinimizer(I.G. Tsoulos et al., 2023).|             
| **Parallel Particle Swarm optimizer**  | proposed in to develop a parallel PSO variant. The method is denoted as ParallelPso in the Optimus package(V. Charilogis et al.,  2023).|  
| **Simulated annealing optimizer**  | included in the software under the name Simman. |     
| **The optimal foraging algorithm (OFA)**  | motivated by animal behavioral ecology included the software named Ofa (Kyrou, G. et al., 2024).|    
| **Bio-inspired metaheuristic algorithm Giant Armadillo Optimization (GAO)** |  mimics the natural behavior of the giant armadillo  included in the software named armadillo1(Kyrou G. et al., 2024). | 
| **The Gray Wolf Optimizer (GWO)** |  mimics hunting mechanism of gray wolves in nature  included in the software named Gwo. |

###  Implemented local optimization methods 
<p style="text-align: justify;">
The parameter used to determine the used local optimization procedure is the −−opt_localsearch parameter. The implemented local optimization methods are the following: 
</p>

METHOD |  DESCRIPTION |
|----------|----------|
|  **The bfgs method**        | The Broyden–Fletcher–Goldfarb–Shanno (BFGS) algorithm was implemented using a variant of Powell.|       
|  **The lbfgs method**      | The limited memory BFGS method is implemented as an approximation of the BFGS method using a limited amount of computer memory |                         
|**The Gradient descent method** | This method is denoted as gradient in the software and  implements the Gradient Descent local optimization   procedure. T (I.G. Tsoulos et al., 2008) etc.|          
| **The Nelder Mead method**  |The Nelder Mead simplex procedure for local optimization  is also included in the software and it is denoted as nelderMead.   |                           
|  **The adam method**  |  The adam  optimizer  |

### Implementing a user - defined optimization method
<p style="text-align: justify;">
The software can be extended by implementing optimization techniques by the user himself. For this purpose, there is the optimization method named UserMethod and the user can implement the provided functions according to the requirements of the method.
</p>

Method addParam()  |  DESCRIPTION |
|----------|----------|
|    **init()**     |This function is called every time the optimization method starts|                                    
|    **step()**    |This function implements the actual step of the optimization method. |                                            
|    **terminated()**    |This function is used as the termination step of the optimization method.|                                   
|    **done()**    |   This function will be called when the optimization method terminates.|                                       
|   **~UserMethod()**     |    This is the destructor of the optimization method.  |                                            

A flowchart of any used optimization method is outlined in Figure \autoref{userMethod}.


![usermethod] \label{userMethod}(usermethod.png)



**Figure 1.** The flowchart of the execution steps of the optimization methods.



###  Conclusions 
<p style="text-align: justify;">
In this work, an environment for executing global optimization problems was presented. In this environment, the user can code the objective problem using some predefined functions and then has the possibility to choose one among several global optimization methods to solve the mentioned problem. In addition, it is given the possibility to choose to use some local optimization method to enhance the reliability of the produced results. This programming environment is freely available and easy to extend to accommodate more global optimization techniques. 
</p>



# References
I.G. Tsoulos, A. Tzallas, D. Tsalikakis (2016). _PDoublePop: An implementation of parallel genetic algorithm for function optimization_, Computer Physics Communications 209, pp. 183-189

V. Charilogis, I.G. Tsoulos. (2023).  _A Parallel Implementation of the Differential Evolution Method_, Analytics 2, pp. 17-30.

I.G. Tsoulos (2008).  _Modifications of real code genetic algorithm for global optimization,_ Applied Mathematics and Computation 203, pp. 598-607.

V. Charilogis, I.G. Tsoulos (2022). _Toward an Ideal Particle Swarm Optimizer for Multidimensional Functions_, Information 13, 217.

I.G. Tsoulos, A. Tzallas, E. Karvounis, D. Tsalikakis(2023). _NeuralMinimizer, a novel method for global optimization that incorporates machine learning_, Information 14, 2.

V. Charilogis, I.G. Tsoulos, A. Tzallas (2023).  _An Improved Parallel Particle Swarm Optimization_, SN COMPUT. SCI. 4, 766.

Kyrou, G., Charilogis, V., & Tsoulos, I. G. (2024). _EOFA: An Extended Version of the Optimal Foraging Algorithm for Global Optimization Problems._ Computation, 12(8), 158.

Kyrou, G., Charilogis, V., & Tsoulos, I. G. (2024). _Improving the Giant-Armadillo Optimization Method._ Analytics, 3(2), 225-240.

I.G. Tsoulos, I.E. Lagaris (2008).  _GenMin: An enhanced genetic algorithm for global optimization_, Computer Physics Communications 178, pp. 843-851.
