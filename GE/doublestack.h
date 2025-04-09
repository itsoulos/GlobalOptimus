# ifndef __DOUBLESTACK__H

# include <stdlib.h>
# include <stdio.h>
# include <math.h>
# include <string.h>

/**
 * @brief The DoubleStack class implements a stack for
 * double precision values.
 */
class DoubleStack
{
	private:
		double *data;
		int counter;
	public:
        /**
         * @brief DoubleStack implements the constructor of the class.
         */
		DoubleStack();
        /**
         * @brief size
         * @return the size of the stack.
         */
		int  size() const;
        /**
         * @brief push Pushes a new value to the stack.
         * @param x
         */
		void push(double x);
        /**
         * @brief top
         * @return the top of the stack without removing the value.
         */
		double  top() const;
        /**
         * @brief pop
         * @return the top of the stack
         */
		double pop();
        /**
         * @brief clear Clears the stack.
         */
		void	clear();
		~DoubleStack();
};

# define __DOUBLESTACK__H
# endif
