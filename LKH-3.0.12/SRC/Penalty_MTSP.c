#include "LKH.h"
#include "Segment.h"

GainType Penalty_MTSP_MINSUM()
{
    int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;
    Node *N = Depot, *NextN;
    GainType P = 0, DistanceSum;

    do {
        int Size = -1;
        do {
            Size++;
            NextN = Forward ? SUCC(N) : PREDD(N);
            if (NextN->Id > DimensionSaved)
                NextN = Forward ? SUCC(NextN) : PREDD(NextN);
        } while ((N = NextN)->DepotId == 0);
        if (MTSPMaxSize < Dimension - Salesmen && Size > MTSPMaxSize)
            P += Size - MTSPMaxSize;
        if (MTSPMinSize >= 1 && Size < MTSPMinSize)
            P += MTSPMinSize - Size;
    } while (N != Depot);
    if (DistanceLimit != DBL_MAX) {
        do {
            if (P > CurrentPenalty ||
                (P == CurrentPenalty && CurrentGain <= 0))
                return CurrentPenalty + (CurrentGain > 0);
            DistanceSum = 0;
            do {
                NextN = Forward ? SUCC(N) : PREDD(N);
                DistanceSum += (C(N, NextN) - N->Pi - NextN->Pi) /
                    Precision;
                if (NextN->Id > DimensionSaved)
                    NextN = Forward ? SUCC(NextN) : PREDD(NextN);
            } while ((N = NextN)->DepotId == 0);
            if (DistanceSum > DistanceLimit)
                P += DistanceSum - DistanceLimit;
        } while (N != Depot);
    }
    return P;
}

#ifdef CAVA_PENALTY

#define ARE_LINKED(N1, N2) (N1->Suc == N2 || N1->Pred == N2)

static GainType oldPenaltySum;
GainType Penalty_MTSP_MINMAX_Old(void);            /* Original O(N) Penalty function*/
static void update_Penalty_MTSP(void);      /* O(N) update step called if an improvement is found to updated all route metadata.*/
static int setup_Node_MTSP(Node *);         /* Utility function: count the old penalty sum given the route of the node.*/
static int setup_Penalty_MTSP(void);        /* Compute the previous penalty sum. */
static int was_empty_route(Node *, Node *); /* Used to test *only once* if a removed edge (from a 2opt move) was the edge of an empty 
                                                route (depot->depot). After the first check, it return 0 until the relative
                                                flags is reseted (To avoid counting multiple times the same route).*/

GainType Penalty_MTSP_MINMAX()
{
    GainType P = 0;
    if (Swaps && cava_PetalsData)
    {
        GainType Cost;
        Node *N;
        //Moves that only touch one route cannot change the penalty value
        if (setup_Penalty_MTSP() == 1)
            return CurrentPenalty;
        for (SwapRecord *si = SwapStack + Swaps - 1; si >= SwapStack; --si)
        {
            // Test for empty routes
            if ((si->t1->DepotId && si->t4->DepotId && ARE_LINKED(si->t1, si->t4)) ||
                (si->t2->DepotId && si->t3->DepotId && ARE_LINKED(si->t2, si->t3)))
                P += MTSPMinSize;
            for (int twice = 0; twice < 2; ++twice)
            {
                if (twice > 0)
                    N = si->t2->PFlag ? si->t2 : si->t3;
                else
                    N = si->t1->PFlag ? si->t1 : si->t4;
                if (N->PFlag)
                {
                    Cost = 0;
                    Node *savedN = N;
                    N->PFlag = 0;
                    //Forward
                    while ((N = SUC(N))->DepotId == 0)
                    {
                        N->PFlag = 0;
                        Cost += C(N, SUCC(N)) - N->Pi - SUCC(N)->Pi;
                    }
                    GainType tempP = P + Cost / Precision;
                    if (Cost > CurrentPenalty ||
                        (Cost == CurrentPenalty && CurrentGain <= 0))
                    {
                        for (SwapRecord *s = si - 1; s >= SwapStack; --s)
                            s->t1->PFlag = s->t2->PFlag = s->t3->PFlag = s->t4->PFlag = 0;

                        return CurrentPenalty + (CurrentGain > 0);
                    }
                    //Backward
                    N = savedN;
                    while ((N = PRED(N))->DepotId == 0)
                    {
                        N->PFlag = 0;
                        Cost += C(N, PRED(N)) - N->Pi - PRED(N)->Pi;
                    }
                    if (Cost > CurrentPenalty ||
                        ((P += Cost / Precision) > oldPenaltySum ||
                         (P == oldPenaltySum && CurrentGain <= 0)))
                    {
                        for (SwapRecord *s = si - 1; s >= SwapStack; --s)
                            s->t1->PFlag = s->t2->PFlag = s->t3->PFlag = s->t4->PFlag = 0;

                        return CurrentPenalty + (CurrentGain > 0);
                    }
                }
            }
        }
        if (!CurrentPenalty)
            return P;
        if (P < oldPenaltySum ||
            (P == oldPenaltySum && CurrentGain > 0))
        {
            update_Penalty_MTSP(); //Improved!
            return CurrentPenalty + P - oldPenaltySum;
        }
        else
            return CurrentPenalty + (CurrentGain > 0);
    }
    else
    {
        P = Penalty_MTSP_MINMAX_Old();
        if (P < CurrentPenalty ||
            (P == CurrentPenalty && CurrentGain > 0))
        {
            if (!cava_PetalsData)
                cava_PetalsData = (RouteData *)calloc(Salesmen + 1, sizeof(RouteData));
            update_Penalty_MTSP();
        }
        return P;
    }
}

/* Returns 1 if only one route is involved in the current move */
static int setup_Penalty_MTSP()
{
    oldPenaltySum = 0;
    int petalCounter = 0;
    if (CurrentPenalty)
    {
        for (SwapRecord *s = SwapStack + Swaps - 1; s >= SwapStack; --s)
        {
            //If a move has involved the edge of an empty route an additional empty one needs to be counted
            Node *t1 = s->t1, *t2 = s->t2, *t3 = s->t3, *t4 = s->t4;

            if ((!ARE_LINKED(t1, t2) && was_empty_route(t1, t2)) ||
                (!ARE_LINKED(t3, t4) && was_empty_route(t3, t4)))
            {
                ++petalCounter;
                oldPenaltySum += MTSPMinSize;
            }
            petalCounter += setup_Node_MTSP(t1) + setup_Node_MTSP(t2) +
                            setup_Node_MTSP(t3) + setup_Node_MTSP(t4);
        }
        //Reset petals flags for next petal counting
        for (SwapRecord *s = SwapStack + Swaps - 1; s >= SwapStack; --s)
        {
            Node *t1 = s->t1, *t2 = s->t2, *t3 = s->t3, *t4 = s->t4;
            int d1 = t1->DepotId, d2 = t2->DepotId, d3 = t3->DepotId, d4 = t4->DepotId;

            cava_PetalsData[d1].flag = cava_PetalsData[d2].flag =
                cava_PetalsData[d3].flag = cava_PetalsData[d4].flag = 0;

            t1->PetalId->flag = t2->PetalId->flag = t3->PetalId->flag = t4->PetalId->flag = 0;
        }
        if (petalCounter == 1)
            return 1;
    }
    for (SwapRecord *s = SwapStack + Swaps - 1; s >= SwapStack; --s)
    {
        s->t1->PFlag = !s->t1->DepotId;
        s->t2->PFlag = !s->t2->DepotId;
        s->t3->PFlag = !s->t3->DepotId;
        s->t4->PFlag = !s->t4->DepotId;
    }
    return petalCounter;
}

static int was_empty_route(Node *N1, Node *N2)
{
    int *f1 = &cava_PetalsData[N1->DepotId].flag;
    int *f2 = &cava_PetalsData[N2->DepotId].flag;
    return (!*f1 && (*f1 |= (cava_PetalsData[N1->DepotId].minNode == N2))) ||
           (!*f2 && (*f2 |= (cava_PetalsData[N2->DepotId].minNode == N1)));
}

static int setup_Node_MTSP(Node *N)
{
    if (!N->PetalId->flag)
    {
        oldPenaltySum += N->PetalId->OldPenalty;
        N->PetalId->flag = 1;
        return (N->PetalId != cava_PetalsData); //Depots have PetalId_index == 0
    }
    return 0;
}

/* Update route data when a new improving tour is found */
static void update_Penalty_MTSP()
{
    Node *N = Depot;
    RouteData *CurrId;
    GainType Cost;
    int Size;
    do
    {
        Cost = Size = 0;
        N->PetalId = cava_PetalsData; //depots point to 0 cell
        CurrId = cava_PetalsData + N->DepotId;
        while ((N = SUCC(N))->DepotId == 0)
        {
            ++Size;
            N->PetalId = CurrId;
            Cost += C(N, SUCC(N)) - N->Pi - SUCC(N)->Pi;
        }
        CurrId->OldPenalty = Cost / Precision;
        CurrId->minNode = Size ? NULL : N; /*Save the adjacent depot to recognize empty routes*/
    } while (N != Depot);
}

GainType Penalty_MTSP_MINMAX_Old()
#else
GainType Penalty_MTSP_MINMAX()
#endif
{
    int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;
    static Node *StartRoute = 0;
    Node *N, *NextN, *CurrentRoute;
    GainType Cost, MaxCost = MINUS_INFINITY;

    if (!StartRoute)
        StartRoute = Depot;
    if (StartRoute->Id > DimensionSaved)
        StartRoute -= DimensionSaved;
    N = StartRoute;
    do {
        Cost = 0;
        CurrentRoute = N;
        do {
            NextN = Forward ? SUCC(N) : PREDD(N);
            Cost += C(N, NextN) - N->Pi - NextN->Pi;
            if (NextN->Id > DimensionSaved)
                NextN = Forward ? SUCC(NextN) : PREDD(NextN);
        } while ((N = NextN)->DepotId == 0);
        Cost /= Precision;
        if (Cost > MaxCost) {
            if (Cost > CurrentPenalty ||
                (Cost == CurrentPenalty && CurrentGain <= 0)) {
                StartRoute = CurrentRoute;
                return CurrentPenalty + (CurrentGain > 0);
            }
            MaxCost = Cost;
        }
    } while (N != StartRoute);
    return MaxCost;
}

GainType Penalty_MTSP_MINMAX_SIZE()
{
    int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;
    static Node *StartRoute = 0;
    Node *N, *NextN, *CurrentRoute;
    int Size, MaxSize = INT_MIN;

    if (!StartRoute)
        StartRoute = Depot;
    if (StartRoute->Id > DimensionSaved)
        StartRoute -= DimensionSaved;
    N = StartRoute;
    do {
        Size = 0;
        CurrentRoute = N;
        do {
            NextN = Forward ? SUCC(N) : PREDD(N);
            Size++;
            if (NextN->Id > DimensionSaved)
                NextN = Forward ? SUCC(NextN) : PREDD(NextN);
        } while ((N = NextN)->DepotId == 0);
        if (Size > MaxSize) {
            if (Size > CurrentPenalty ||
                (Size == CurrentPenalty && CurrentGain <= 0)) {
                StartRoute = CurrentRoute;
                return CurrentPenalty + (CurrentGain > 0);
            }
            MaxSize = Size;
        }
    } while (N != StartRoute);
    return MaxSize;
}
