---
title: 'OPTIMUS: a multidimensional global optimization package'
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
bibliography: paper.bib

---

# Summary
<p style="text-align: justify;">
The location of global minimum founds application in a variety of problems derived from physics [@duan1992effective_1992], chemistry [@liwo1999protein_1999], medicine [@lee2007large_2007] etc. Also, the global optimization process can be used in complex problems, such as the training of neural networks [@chiroma2017neural_2017]. Methods used in the global optimization process can be divided into stochastic and deterministic, and  a comparison between methods of these categories can be found in the work of Sergeyev et al.[@sergeyev2018efficiency_2018]. This article proposes a software that incorporates a variety of stochastic methods to handle optimization methods of multidimensional functions.
</p>


# Statement of need
<p style="text-align: justify;">
This paper presents the `GlobalOptimus` software tool,  written in ANSI C++, which researchers can use to formulate the problem to be solved and then apply the required global optimization method identify the global minimum of the problem. The main features of the suggested software are: a) Coding of the objective problem in a high level language such as ANSI C++ b) Incorporation of many global optimization techniques to tackle the objective problem c)Parameterization of global optimization methods using user-defined parameters d) Usage of a GUI application to control the optimization strategy.
</p>

##  Software 
<p style="text-align: justify;">
The software can be compiled using the QT library, available from  https://qt.io. The researcher should code the objective function and a number of other mandatory functions in the C++ programming language. Also, the researcher should provide the dimension of the objective function as well as the bound of the function. Subsequently, the user can select a global optimization method to apply to the problem from a wide range of available methods.</p>

## Implemented global optimization methods
<p style="text-align: justify?">
 In the proposed software, each implemented global optimization method has a set of parameters that can determine the overall optimization path and the effectiveness of the method. Some of the global optimization methods are as follows: </p>
 

METHOD |  DESCRIPTION |
|----------|----------|
| **Differential Evolution**   |An evolutionary algorithm used  in many areas [@pant2020differential_2020]|
| **Parallel Differential Evolution**| A parallel version of the Differential Evolution method [@charilogis2023parallel_2023].|
| **Double precision genetic algorithm**  | An Improved version of the Genetic Algorithm [@tsoulos2008modifications_2008].|
| **Improved Particle Swarm Optimization**  |Suggested as an improvement of the PSO method [@charilogis2022toward_2022].|
| **Multistart**  |Initiates local searches from different start points [@marti2010advanced_2010].|
| **NeuralMinimizer**| Utilizes Radial Basis Functions (RBF) networks to increase the speed of the Multistart method [@tsoulos2023neuralminimizer_2023].|
| **Parallel Particle Swarm optimizer**  | A novell parallel PSO variant, published recently [@charilogis2023improved_2023].|
| **Simulated annealing optimizer**  | included in the software under the name Simman [@kirkpatrick1983optimization_1983]. |
| **The optimal foraging algorithm (OFA)**  | motivated by animal behavioral ecology included the software named Ofa [@kyrou2024eofa_2024].|
| **Bio-inspired metaheuristic algorithm Giant Armadillo Optimization (GAO)** |  This process mimics the natural behavior of the giant armadillo [@kyrou2024improving_2024]. |
| **The Gray Wolf Optimizer (GWO)** |  This process mimics hunting mechanism of gray wolves in nature [@li2021improved_2021].|

##  Implemented local optimization methods
<p style="text-align: justify;">
The parameter used to determine the used local optimization procedure is the −−opt_localsearch parameter. The implemented local optimization methods are the following: 
</p>

METHOD |  DESCRIPTION |
|----------|----------|
|  **The bfgs method**        | The Broyden–Fletcher–Goldfarb–Shanno (BFGS) algorithm was implemented [@powell1989tolerant_1989].|
|  **The lbfgs method**      | The limited memory BFGS method is implemented as an approximation of the BFGS method using a limited amount of computer memory [@liu1989limited_1989]. |
|**The Gradient descent method** | This method is denoted as gradient in the software and  implements the Gradient Descent local optimization   procedure [@amari1993backpropagation_1993].|
| **The Nelder Mead method**  |The Nelder Mead simplex procedure for local optimization  is also included in the software [@olsson1975nelder_1975].   |
|  **The adam method**  |  The adam  optimizer [@kingma2014adam_2014].  |

## Implementing a user - defined optimization method
<p style="text-align: justify;">
The software can be extended by implementing optimization techniques by the user himself. For this purpose, there is the optimization method named UserMethod and the user can implement the provided functions according to the requirements of the method.
</p>

Method   |  DESCRIPTION |
|----------|----------|
|    **init()**     |This function is called every time the optimization method starts|                                    
|    **step()**    |This function implements the actual step of the optimization method. |                                            
|    **terminated()**    |This function is used as the termination step of the optimization method.|                                   
|    **done()**    |   This function will be called when the optimization method terminates.|                                       
|   **~UserMethod()**     |    This is the destructor of the optimization method.  |                                            

A flowchart of any used optimization process is outlined in Figure \autoref{FIG:optimization}.


![The diagram of the optimization process \label{FIG:optimization}](usermethod.png)

Also, an example run for the function rastrigin defined as
$ f(x)=x_1^{2}+x_2^{2}-\cos(18x_1)-\cos(18x_2)$  and the Genetic algorithm is outlined in Figure \autoref{FIG:rastrigin}.

![An example run for the function rastrigin \label{FIG:rastrigin}](rastrigin.png)

#  Conclusions
<p style="text-align: justify;">
In this work, an environment for executing global optimization problems was presented. In this environment, the user can code the objective problem using some predefined functions and then has the possibility to choose one among several global optimization methods to solve the mentioned problem. In addition, it is given the possibility to choose to use some local optimization method to enhance the reliability of the produced results. This programming environment is freely available and easy to extend to accommodate more global optimization techniques. 
</p>

# References
