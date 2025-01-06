#ifndef NNCPROGRAM_H
#define NNCPROGRAM_H
# include <GE/cprogram.h>

class NncProgram : public Cprogram
{
    private:
      Symbol Sig,Out,s0,sx,smallNumber;
      virtual void			makeRules();
    public:
        NncProgram(int dim,Model *m);
        virtual ~NncProgram();
};

#endif // NNCPROGRAM_H
