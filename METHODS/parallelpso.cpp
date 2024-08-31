#include <METHODS/parallelpso.h>
#include <algorithm>
# include "omp.h"
# include <QRandomGenerator>
ParallelPso::ParallelPso()
{
    before = std::chrono::system_clock::now();

    addParam(Parameter("parallelPso_particles", 100,10,10000, "Number of pso particles"));
    addParam(Parameter("parallelPso_generations", 200,10,20000, "Maximum number of pso generations"));
    addParam(Parameter("parallelPso_c1", 0.5,0.0,2.0, "Pso c1 parameter"));
    addParam(Parameter("parallelPso_c2", 0.5,0.0,2.0, "Pso c2 parameter"));
    addParam(Parameter("similarityMaxCount", 15,1,100, "Maximum allowed itearations for Similarity Stopping rule"));
    addParam(Parameter("parallelPso_propagateRate", 15,1,100, "The number of generations before the propagation takes place"));
    QStringList parallelPso_propagateMethod;
    parallelPso_propagateMethod<<"1to1"<<"1toN"<<"Nto1"<<"NtoN";
    addParam(Parameter("parallelPso_propagateMethod", parallelPso_propagateMethod[0],
                       parallelPso_propagateMethod,
                       "The propagation method used. Available values: 1to1,1toN,Nto1,NtoN"));
    addParam(Parameter("parallelPso_subCluster", 5,1,100, "number of subclusters for pso"));
    addParam(Parameter("parallelPso_subClusterEnable", 1,1,100, "the number of subclusters that play a role in the termination rule: [1, islands]"));
    //addParameter("pRate", "0", "the rate of particles for propagation"));
    addParam(Parameter("parallelPso_pNumber", 0,0,100, "the number of particles for propagation"));
}

void ParallelPso::getBestValue(int &index, double &value)
{
    value = 1e+100;
    index = 0;
    for (int i = 0; (int)i < particles.size(); i++)
    {
        if (fitness_array[i] < value || i == 0)
        {
            value = fitness_array[i];
            index = i;
        }
    }
}

void ParallelPso::getBestValue(int subClusterName, int &index, double &value)
{
    value = 1e+100;
    index = 0;
    int i = subClusterStartPos(subClusterName);
    int end = this->subClusterEndPos(subClusterName);
    for (; (int)i < end; i++)
    {
        if (fitness_array.at(i) < value)
        {
            value = fitness_array.at(i);
            index = i;
        }
    }
}

int ParallelPso::subClusterStartPos(int subClusterIndex)
{
    return subClusterIndex * particles.size() / subCluster;
}

int ParallelPso::subClusterEndPos(int subClusterIndex)
{
    return (subClusterIndex + 1) * particles.size() / subCluster - 1;
}

bool sortByFirstDesc(const pair<double, int> &a, const pair<double, int> &b)
{
    return a.first < b.first;
}
void ParallelPso::replace(int subClusterIndex, vector<pair<double, Data>> proParticles)
{
    /*
        std::cout<<"best"<<endl;
        for (int i=0;i< proParticles.size();i++)
        {
            std::cout<<proParticles.at(i).first<<endl;
        }
    */
    vector<pair<double, int>> tmp;
    tmp.resize(parallelPsoParticles);
    for (int i = 0; i < parallelPsoParticles; i++)
    {
        tmp.at(i).first = -1e+100;
        tmp.at(i).second = 0;
    }
    for (int i = subClusterStartPos(subClusterIndex), j = 0; i <= subClusterEndPos(subClusterIndex); i++, j++)
    {
        if (tmp.at(j).first < fitness_array[i])
        {
            tmp.at(j).first = fitness_array[i];
            tmp.at(j).second = i;
        }
    }
    sort(tmp.begin(), tmp.end(), sortByFirstDesc);
    tmp.resize(pNumber);
    /*
        std::cout<<"worst"<<endl;
        for (int i=0;i< pNumber;i++)
        {
            std::cout<<tmp.at(i).first<<endl;
        }
    */
    vector<int> tmp2;
    for (int i = 0; i < pNumber; i++)
    {
        tmp2.push_back(tmp.at(i).second);
    }
    for (int i = subClusterStartPos(subClusterIndex); i <= subClusterEndPos(subClusterIndex); i++)
    {
        if (find(tmp2.begin(), tmp2.end(), i) != tmp2.end())
        {
            fitness_array[i] = proParticles.back().first;
            particles[i] = proParticles.back().second;
            proParticles.pop_back();
        }
    }
}

void ParallelPso::propagate()
{
    if (pNumber > 0 && subCluster > 1 )
    {
        if (parallelPropagateMethod == "1to1")
        {
            int subCluster1, subCluster2;
            do
            {
                subCluster1 = rand() % subCluster;
                subCluster2 = rand() % subCluster;
            } while (subCluster1 == subCluster2);

            vector<pair<double, Data>> tmp;
            tmp.resize(parallelPsoParticles);

            for (int i = 0; i < parallelPsoParticles; i++)
                tmp.at(i).first = 1e+100;

            for (int i = subClusterStartPos(subCluster1), j = 0; i <= subClusterEndPos(subCluster1); i++, j++)
            {
                if (tmp.at(j).first > fitness_array[i])
                {
                    tmp.at(j).first = fitness_array[i];
                    tmp.at(j).second = particles[i];
                }
            }
            sort(tmp.begin(), tmp.end());
            tmp.resize(pNumber);
            replace(subCluster2, tmp);
        }
        else if (parallelPropagateMethod == "1toN")
        {
            int subCluster1 = rand() % subCluster;
            vector<pair<double, Data>> tmp;

            for (int i = 0; i < subCluster; i++)
            {
                tmp.resize(parallelPsoParticles);
                for (int k = 0; k < parallelPsoParticles; k++)
                    tmp.at(k).first = 1e+100;
                if (i == subCluster1)
                    continue;
                for (int i = subClusterStartPos(subCluster1), j = 0; i <= subClusterEndPos(subCluster1); i++, j++)
                {
                    if (tmp.at(j).first > fitness_array[i])
                    {
                        tmp.at(j).first = fitness_array[i];
                        tmp.at(j).second = particles[i];
                    }
                }
                sort(tmp.begin(), tmp.end());
                tmp.resize(pNumber);
                replace(i, tmp);
            }
        }
        else if (parallelPropagateMethod == "Nto1")
        {
            int subCluster2 = rand() % subCluster;
            vector<pair<double, Data>> tmp;
            for (int i = 0; i < subCluster; i++)
            {
                tmp.resize(parallelPsoParticles);
                for (int k = 0; k < parallelPsoParticles; k++)
                    tmp.at(k).first = 1e+100;
                if (i == subCluster2)
                    continue;
                int subCluster1 = i;
                for (int k = subClusterStartPos(subCluster1), j = 0; k <= subClusterEndPos(subCluster1); k++, j++)
                {
                    if (tmp.at(j).first > fitness_array[k])
                    {
                        tmp.at(j).first = fitness_array[k];
                        tmp.at(j).second = particles[k];
                    }
                }
                sort(tmp.begin(), tmp.end());
                tmp.resize(pNumber);
                replace(subCluster2, tmp);
            }
        }
        else if (parallelPropagateMethod == "NtoN")
        {
            vector<pair<double, Data>> tmp;
            for (int i = 0; i < subCluster; i++)
            {
                for (int j = 0; j < subCluster; j++)
                {
                    tmp.resize(parallelPsoParticles);
                    for (int k = 0; k < parallelPsoParticles; k++)
                        tmp.at(k).first = 1e+100;
                    if (i == j)
                        continue;
                    int subCluster1 = i;
                    int subCluster2 = j;
                    for (int k = subClusterStartPos(subCluster1), e = 0; k <= subClusterEndPos(subCluster1); k++, e++)
                    {
                        if (tmp.at(e).first > fitness_array[k])
                        {
                            tmp.at(e).first = fitness_array[k];
                            tmp.at(e).second = particles[k];
                        }
                    }
                    sort(tmp.begin(), tmp.end());
                    tmp.resize(pNumber);
                    replace(subCluster2, tmp);
                }
            }
        }
    }
}

bool ParallelPso::terminated()
{
    int c = 0;
    for (int i = 0; i < subCluster; i++)
        if (this->checkSubCluster(i))
            c++;
    return generation >= parallelPsoGenerations || c >= subClusterEnable;
}

bool ParallelPso::checkSubCluster(int subClusterName)
{
    //double difference = fabs(bestF2xInClusterOLD.at(subClusterName) - bestF2xInCluster.at(subClusterName));
    parallelPso_termination = terminationMethod;
    if(parallelPso_termination == "doublebox" && doubleBox.terminate(fabs(bestF2xInClusterOLD.at(subClusterName) - bestF2xInCluster.at(subClusterName)))) return true;
    if(parallelPso_termination == "similarity" && similarity.terminate(fabs(bestF2xInClusterOLD.at(subClusterName) - bestF2xInCluster.at(subClusterName)))) return true; //similarity
    if(generation>=parallelPsoGenerations) return true;

    return false;
    /*
        getBestValue(subClusterName, bestIndex, bestValue);
        //---------------------SIMILARITY CASE 1 QUANTITIES------------------------------

        double difference = fabs(newSum.at(subClusterName) - sum.at(subClusterName));
        // printf("%d] neWsum.at(%d) : %f Sum.at(%d) : %f different  : %lf\n", generation, subClusterName, newSum.at(subClusterName), subClusterName, sum.at(subClusterName), subClusterName, difference );
        sum.at(subClusterName) = newSum.at(subClusterName);

        if (difference < 1e-8)
            similarityCurrentCount.at(subClusterName)++;
        else
            similarityCurrentCount.at(subClusterName) = 0;
        if (similarityCurrentCount.at(subClusterName) >= similarityMaxCount)
            return true;
        //---------------------SIMILARITY CASE 2 AVERAGE TERMS----------------------------

        double difference = fabs(newMO.at(subClusterName) - MO.at(subClusterName));
        // printf("%d] newMO.at(%d) : %f MO.at(%d) : %f different  : %lf\n", generation, subClusterName, newMO.at(subClusterName), subClusterName, MO.at(subClusterName), subClusterName, difference );
        MO.at(subClusterName) = newMO.at(subClusterName);

        if (difference < 1e-8)
            similarityCurrentCount.at(subClusterName)++;
        else
            similarityCurrentCount.at(subClusterName) = 0;
        if (similarityCurrentCount.at(subClusterName) >= similarityMaxCount)
            return true;
        //--------------------------------------------------------------------------------
    */
    //----------------SIMILARITY CASE 3 DIFFERANCE F2X WITH F2X OLD-------------------

    double difference = fabs(bestF2xInClusterOLD.at(subClusterName) - bestF2xInCluster.at(subClusterName));
    // if (bestF2xInClusterOLD.at(subClusterName) == bestF2xInCluster.at(subClusterName))
    if (difference < 1e-6)
        similarityCurrentCount.at(subClusterName)++;
    else
        similarityCurrentCount.at(subClusterName) = 0;
    if (similarityCurrentCount.at(subClusterName) >= similarityMaxCount)
        return true;
    /*
        double bestValue;
        int bestIndex;
        doublebox_xx1.at(subClusterName) += bestValue;
        doublebox_xx2.at(subClusterName) += bestValue * bestValue;
        doublebox_variance.at(subClusterName) = doublebox_xx2.at(subClusterName) / generation - (doublebox_xx1.at(subClusterName) / generation) * (doublebox_xx1.at(subClusterName) / generation);
        if (bestValue < doublebox_best_value.at(subClusterName))
        {
            doublebox_best_value.at(subClusterName) = bestValue;
            doublebox_stopat.at(subClusterName) = doublebox_variance.at(subClusterName) / 2.0;
        }
        printf("%4d] doublebox_variance.at(%d) : %f doublebox_stopat.at(%d) : %f different  : %lf\n", generation, subClusterName, doublebox_variance.at(subClusterName), subClusterName, doublebox_stopat.at(subClusterName), subClusterName, fabs(doublebox_variance.at(subClusterName) - doublebox_stopat.at(subClusterName)));
        return doublebox_variance.at(subClusterName) <= doublebox_stopat.at(subClusterName);
    */
    return false;
}

void ParallelPso::step()
{

    ++generation;
    int t, i, j;
#pragma omp parallel for private(i, j) num_threads(subCluster)
    for (t = 0; t < subCluster; t++)
    {

        bestF2xInClusterOLD.at(t) = bestF2xInCluster.at(t);
        // #pragma omp parallel for  private (t, i, j) num_threads(subCluster)
        for (i = subClusterStartPos(t); i <= subClusterEndPos(t); i++)
        {
            QRandomGenerator gen1 = QRandomGenerator();
            QRandomGenerator gen2 = QRandomGenerator();
            QRandomGenerator gen3 = QRandomGenerator();
            double R, inertia, r1, r2, tj, part1, part2, part3, trialf;
            bool flag = false;
            // printf("subClusterStartPos(%d)\tsubClusterEndPos(%d)\n",subClusterStartPos(t),subClusterEndPos(t-1));
            R = gen1.bounded(1.0);
            inertia = 0.5 + (R / 2.0);

            for (j = 0; j < dimension; j++)
            {
                r1 = gen2.bounded(1.0);
                r2 = gen3.bounded(1.0);
                tj = velocitys[i][j];
                part1 = inertia * velocitys[i][j];
                part2 = parallelPsoC1 * r1 * (bestParticle[i][j] - particles[i][j]);
                part3 = parallelPsoC2 * r2 * (bestParticleInCluster[t][j] - particles[i][j]);
                velocitys[i][j] = part1 + part2 + part3;
                trialf = particles[i][j] + velocitys[i][j];
                if (trialf < lmargin[j] || trialf > rmargin[j])
                {
                    velocitys[i][j] = tj;
                    continue;
                }
                else
                {
                    particles[i][j] = trialf;
                    flag = true;
                }
            }
            if (flag)
                fitness_array[i] = myProblem->statFunmin(particles[i]);
#pragma omp critical
            {
                if (best_fitness_array[i] > fitness_array[i])
                {
                    best_fitness_array[i] = fitness_array[i];
                    bestParticle[i] = particles[i];
                }
                if (besty > best_fitness_array[i])
                {
                    besty = fitness_array[i];
                    bestx = particles[i];
                }
                if (bestF2xInCluster.at(t) > fitness_array[i])
                {
                    bestF2xInCluster.at(t) = fitness_array[i];
                    bestParticleInCluster.at(t) = particles[i];
                }
            }
        }
    }

    for (int k = 0; k < subCluster; k++)
        newSum.at(k) = accumulate(&fitness_array.at(this->subClusterStartPos(k)), &fitness_array.at(this->subClusterEndPos(k)), 0);

    for (int k = 0; k < subCluster; k++)
    {
        newMO.at(k) = (double)newSum.at(k) / subCluster;
    }
    if (pNumber > 0)
    {
        if (generation % propagateRate)
            propagate();
    }
}

void ParallelPso::init()
{
    parallelPso_termination =terminationMethod;
    pNumber = getParam("parallelPso_pNumber").getValue().toInt();
    subCluster = getParam("parallelPso_subCluster").getValue().toInt();
    subClusterEnable = getParam("parallelPso_subClusterEnable").getValue().toInt();
    parallelPsoParticles = getParam("parallelPso_particles").getValue().toInt();
    parallelPsoGenerations = getParam("parallelPso_generations").getValue().toInt();
    parallelPsoC1 = getParam("parallelPso_c1").getValue().toDouble();
    parallelPsoC2 = getParam("parallelPso_c2").getValue().toDouble();
    parallelPropagateMethod = getParam("parallelPso_propagateMethod").getValue();
    similarityMaxCount = getParam("similarityMaxCount").getValue().toInt();
    propagateRate = getParam("parallelPso_propagateRate").getValue().toInt();

    //pNumber = round((double) pRate * parallelPsoParticles / 100);


    dimension = myProblem->getDimension();
    bestParticle.resize(parallelPsoParticles * subCluster);
    velocitys.resize(parallelPsoParticles * subCluster);
    bestx.resize(dimension);
    lmargin = myProblem->getLeftMargin();
    rmargin = myProblem->getRightMargin();
    particles.resize(parallelPsoParticles * subCluster);
    fitness_array.resize(parallelPsoParticles * subCluster);
    best_fitness_array.resize(parallelPsoParticles * subCluster);
    int N = parallelPsoParticles * subCluster;
    sampleFromProblem(N,particles,fitness_array);

    for (int i = 0; i < N; i++)
    {
        best_fitness_array[i] = fitness_array[i];
    }
    similarityBestValue.resize(subCluster);
    similarityCurrentCount.resize(subCluster);
    generation = 0;
    besty = 1e+100;

    for (int i = 0; i < parallelPsoParticles * subCluster; i++)
    {
        bestParticle[i].resize(dimension);
        velocitys[i].resize(dimension);
        bestParticle[i] = particles[i];

        for (int j = 0; j < dimension; j++)
        {
            double left = -(rmargin[j] - lmargin[j]) / 20.0;
            double right = (rmargin[j] - lmargin[j]) / 20.0;
            velocitys[i][j] = left + myProblem->randomDouble() * (right - left);
            //velocitys[i][j] = 0;
        }
    }
    similarityBestValue.resize(subCluster);
    similarityCurrentCount.resize(subCluster);
    sum.resize(subCluster);
    newSum.resize(subCluster);
    MO.resize(subCluster);
    newMO.resize(subCluster);
    bestParticleInCluster.resize(subCluster);
    bestF2xInCluster.resize(subCluster);
    bestF2xInClusterOLD.resize(subCluster);
    for (int i = 0; i < subCluster; i++)
    {
        bestParticleInCluster.at(i).resize(dimension);
        bestF2xInCluster.at(i) = 1e+100;
        similarityBestValue.at(i) = 1e+100;
        similarityCurrentCount.at(i) = 0;
        sum.at(i) = 0;
        MO.at(i) = 0;
        for (int j = subClusterStartPos(i); j <= subClusterEndPos(i); j++)
        {
            if (bestF2xInCluster.at(i) > fitness_array[j])
            {
                bestF2xInCluster.at(i) = fitness_array[j];
                bestParticleInCluster.at(i) = particles[j];
            }
        }
    }
    omp_set_dynamic(0);
    omp_set_num_threads(subCluster);
}

void ParallelPso::done()
{
    besty = localSearch(bestx);
    after = std::chrono::system_clock::now();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
    auto ms = milliseconds.count();
//    std::cout << "Douration: " << (double)ms / 1000.0 << " sec" << std::endl;
}

double ParallelPso::fitness(Data &x)
{
    if (!myProblem->isPointIn(x))
        return 1e+100;
    return myProblem->statFunmin(x);
}


ParallelPso::~ParallelPso()
{
}


