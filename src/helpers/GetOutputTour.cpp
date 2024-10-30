#include "GetOutputTour.h"

py::array_t<int> GetOutputTour(int *Tour)
{
    FILE *TourFile;
    int i, j, k, n, Forward, a, b;

    if (CurrentPenalty != 0 && MTSPObjective == -1 &&
        ProblemType != CCVRP && ProblemType != TRP &&
        ProblemType != CBTSP && ProblemType != CBnTSP &&
        ProblemType != KTSP && ProblemType != PTSP &&
        ProblemType != MLP && ProblemType != GCTSP && ProblemType != CCCTSP)
        return py::array_t<int>();

    // LKHpy output format
    int position = 0;
    py::array_t<int> LKHOutputTour = py::array_t<int>(DimensionSaved);
    

    n = DimensionSaved;
    for (i = 1; i < n && Tour[i] != MTSPDepot; i++);
    Forward = Asymmetric ||
        Tour[i < n ? i + 1 : 1] < Tour[i > 1 ? i - 1 : Dimension];
    if (ProblemType == CTSP_D)
        Forward = Best_CTSP_D_Direction(Tour);
    for (j = 1; j <= n; j++) {
        if ((a = Tour[i]) <= n)
            LKHOutputTour.mutable_at(position++) = ProblemType != STTSP ? a : NodeSet[a].OriginalId;
        if (Forward) {
            if (++i > n)
                i = 1;
        } else if (--i < 1)
            i = n;
        if (ProblemType == STTSP) {
            b = Tour[i];
            for (k = 0; k < NodeSet[a].PathLength[b]; k++)
                LKHOutputTour.mutable_at(position++) = NodeSet[a].Path[b][k];
        }
    }

    // LKHpy output format
    py::array_t<int> LKHpyOutputTour = py::array_t<int>(Dimension + Salesmen);
    position = 0;
    for (i = 0; i < Dimension; i++) {
        if (LKHOutputTour.at(i) > Dim) {
            LKHpyOutputTour.mutable_at(position++) = 0;
            LKHpyOutputTour.mutable_at(position++) = 0;
        } else {
            LKHpyOutputTour.mutable_at(position++) = LKHOutputTour.at(i) - 1;
        }
    }
    // add 0 to the end of the tour
    LKHpyOutputTour.mutable_at(position++) = 0;

    return LKHpyOutputTour;
}

static int Best_CTSP_D_Direction(int *Tour)
{
    Node *N;
    int *Frq, NLoop, p, n = DimensionSaved, i, j, k;
    GainType P[2] = {0};

    Frq = (int *) malloc((Groups + 1) * sizeof(int));
    for (p = 1; p >= 0; p--) {
        memset(Frq, 0, (Groups + 1) * sizeof(int));
        for (i = 1; i <= n && Tour[i] != MTSPDepot; i++);
        N = Depot;
        NLoop = 1;
        for (j = NLoop; j <= n && NLoop; j++) {
            if (p == 1) {
                if (++i > n)
                    i = 1;
            } else if (--i < 1)
                i = n;
            N = &NodeSet[Tour[i]];
            for (k = 1; k < N->Group - RelaxationLevel; k++) {
                P[p] += Frq[k];
                if (P[p] > CurrentPenalty) {
                    NLoop = 0;
                    break;
                }
            }
            Frq[N->Group]++;
        }
    }
    free(Frq);
    return P[0] < P[1] ?  0 : 1;
}