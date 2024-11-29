#include "LKH.h"
#include "Segment.h"

#define REDUNDANT_CHECK /* ONLY DEBUG: checks old and new and assert they are the same. */

#ifdef CAVA_PENALTY
#define ARE_LINKED(N1, N2) (N1->Suc == N2 || N1->Pred == N2)
static GainType oldPenaltySum;
static GainType oldPenaltyMax;

GainType Penalty_MTSP_MINSUM_old(void);            /* Original O(N) Penalty function*/
static void update_Penalty_MTSP_MINSUM(void);      /* O(N) update step called if an improvement is found to updated all route metadata.*/
static int setup_Node_MTSP_MINSUM(Node *);         /* Utility function: count the old penalty sum given the route of the node.*/
static int setup_Penalty_MTSP_MINSUM(void);        /* Compute the previous penalty sum. */

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
GainType Penalty_MTSP_MINMAX_Old(void);
static void update_Penalty_MTSP_MINMAX(void);
static int setup_Node_MTSP_MINMAX(Node *);
static int setup_Penalty_MTSP_MINMAX(void);

static int was_empty_route(Node *, Node *); /* Used to test *only once* if a removed edge (from a 2opt move) was the edge of an empty 
                                                route (depot->depot). After the first check, it return 0 until the relative
                                                flags is reseted (To avoid counting multiple times the same route).*/
#endif



#ifdef CAVA_PENALTY

#ifdef REDUNDANT_CHECK
static GainType Penalty_MTSP_MINSUM_();
GainType Penalty_MTSP_MINSUM()
{
    assert(CurrentPenalty >= 0);
    GainType P1 = Penalty_MTSP_MINSUM_();
    GainType P2 = Penalty_MTSP_MINSUM_old();
    int accepted1 = P1 < CurrentPenalty || (P1 == CurrentPenalty && CurrentGain > 0);
    int accepted2 = P2 < CurrentPenalty || (P2 == CurrentPenalty && CurrentGain > 0);
    // assert(P1 == P2);
    assert(accepted1 == accepted2);
    assert(P1 >= 0);
    return P1;
}

GainType Penalty_MTSP_MINSUM_()
#else
GainType Penalty_MTSP_MINSUM()
#endif
{
    GainType P = 0;
    if (Swaps && cava_PetalsData && 0)
    {
        GainType DistanceSum;
        Node *N;
        // Moves that only touch one route cannot change the penalty value
        if (setup_Penalty_MTSP_MINSUM() == 1)
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
                    DistanceSum = 0;
                    Node *savedN = N;
                    int Size = 1;
                    N->PFlag = 0;
                    // Forward
                    while ((N = SUC(N))->DepotId == 0)
                    {
                        N->PFlag = 0;
                        DistanceSum += C(N, SUC(N)) - N->Pi - SUC(N)->Pi;
                        ++Size;
                    }
                    GainType tempP = P + DistanceSum / Precision;
                    if (DistanceSum > DistanceLimit &&
                        (tempP > oldPenaltySum ||
                         (tempP == oldPenaltySum && CurrentGain <= 0)))
                    {
                        for (SwapRecord *s = si - 1; s >= SwapStack; --s)
                            s->t1->PFlag = s->t2->PFlag = s->t3->PFlag = s->t4->PFlag = 0;

                        return CurrentPenalty;
                    }
                    // Backward
                    N = savedN;
                    while ((N = PRED(N))->DepotId == 0)
                    {
                        N->PFlag = 0;
                        DistanceSum += C(N, PRED(N)) - N->Pi - PRED(N)->Pi;
                        ++Size;
                    }
                    if (Size < MTSPMinSize)
                        P += MTSPMinSize - Size;

                    if (DistanceSum > DistanceLimit &&
                        ((P += DistanceSum / Precision - DistanceLimit) > oldPenaltySum ||
                         (P == oldPenaltySum && CurrentGain <= 0)))
                    {
                        for (SwapRecord *s = si - 1; s >= SwapStack; --s)
                            s->t1->PFlag = s->t2->PFlag = s->t3->PFlag = s->t4->PFlag = 0;

                        return CurrentPenalty;
                    }
                }
            }
        }
        if (!CurrentPenalty)
            return P;
        if (P < oldPenaltySum ||
            (P == oldPenaltySum && CurrentGain > 0))
        {
            update_Penalty_MTSP_MINSUM(); // Improved!
            return CurrentPenalty + P - oldPenaltySum;
        }
        else
            return CurrentPenalty;
    }
    else
    {
        P = Penalty_MTSP_MINSUM_old();
        if (P < CurrentPenalty ||
            (P == CurrentPenalty && CurrentGain > 0))
        {
            if (!cava_PetalsData)
                cava_PetalsData = (RouteData *)calloc(Salesmen + 1, sizeof(RouteData));
            update_Penalty_MTSP_MINSUM();
        }
        return P;
    }
}

/* Returns 1 if only one route is involved in the current move */
static int setup_Penalty_MTSP_MINSUM()
{
    oldPenaltySum = 0;
    int petalCounter = 0;
    if (CurrentPenalty)
    {
        for (SwapRecord *s = SwapStack + Swaps - 1; s >= SwapStack; --s)
        {
            //If a move has involved the edge of an empty route an additional empty one needs to be counted
            Node *t1 = s->t1, *t2 = s->t2, *t3 = s->t3, *t4 = s->t4;

            // was_empty_route: a route with no customers, just a depot-to-depot link
            if ((!ARE_LINKED(t1, t2) && was_empty_route(t1, t2)) ||
                (!ARE_LINKED(t3, t4) && was_empty_route(t3, t4)))
            {
                ++petalCounter;
                oldPenaltySum += MTSPMinSize;
            }
            petalCounter += setup_Node_MTSP_MINSUM(t1) + setup_Node_MTSP_MINSUM(t2) +
                            setup_Node_MTSP_MINSUM(t3) + setup_Node_MTSP_MINSUM(t4);
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
    // a route with no customers, just a depot-to-depot link
    int *f1 = &cava_PetalsData[N1->DepotId].flag;
    int *f2 = &cava_PetalsData[N2->DepotId].flag;
    return (!*f1 && (*f1 |= (cava_PetalsData[N1->DepotId].minNode == N2))) ||
           (!*f2 && (*f2 |= (cava_PetalsData[N2->DepotId].minNode == N1)));
}

static int setup_Node_MTSP_MINSUM(Node *N)
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
static void update_Penalty_MTSP_MINSUM()
{
    int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;
    Node *N = Depot, *NextN;
    RouteData *CurrId;
    int Size;
    do
    {
        Size = 0;
        N->PetalId = cava_PetalsData; //depots point to 0 cell
        CurrId = cava_PetalsData + N->DepotId;
        NextN = Forward ? SUCC(N) : PREDD(N);
        if (NextN->Id > DimensionSaved)
            NextN = Forward ? SUCC(NextN) : PREDD(NextN);
        while ((N = NextN)->DepotId == 0)
        {
            ++Size;
            N->PetalId = CurrId;
            NextN = Forward ? SUCC(N) : PREDD(N);
            if (NextN->Id > DimensionSaved)
                NextN = Forward ? SUCC(NextN) : PREDD(NextN);
        }
        CurrId->OldPenalty = MTSPMaxSize < Dimension - Salesmen && Size > MTSPMaxSize ? Size - MTSPMaxSize : 0;
        CurrId->OldPenalty += MTSPMinSize >= 1 && Size < MTSPMinSize ? MTSPMinSize - Size : 0;
        CurrId->minNode = Size ? NULL : N; /*Save the adjacent depot to recognize empty routes*/
    } while (N != Depot);
}

GainType Penalty_MTSP_MINSUM_old()
#else
GainType Penalty_MTSP_MINSUM()
#endif
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

#ifdef REDUNDANT_CHECK
static GainType Penalty_MTSP_MINMAX_();
GainType Penalty_MTSP_MINMAX()
{
    assert(CurrentPenalty >= 0);
    GainType P2 = Penalty_MTSP_MINMAX_Old();
    GainType P1 = Penalty_MTSP_MINMAX_();
    int accepted1 = P1 < CurrentPenalty || (P1 == CurrentPenalty && CurrentGain > 0);
    int accepted2 = P2 < CurrentPenalty || (P2 == CurrentPenalty && CurrentGain > 0);
    printff("CurrentPenalty: %d\n", CurrentPenalty);
    printff("P1=%d\n", P1);
    printff("P2=%d\n", P2);
    assert(P1 == P2);
    assert(accepted1 == accepted2);
    assert(P1 >= 0);
    return P1;
}

GainType Penalty_MTSP_MINMAX_()
#else
GainType Penalty_MTSP_MINMAX()
#endif
{
    GainType P = 0;
    if (Swaps && cava_PetalsData)
    {
        GainType DistanceSum;
        Node *N;
        setup_Penalty_MTSP_MINMAX();

        for (SwapRecord *si = SwapStack + Swaps - 1; si >= SwapStack; --si)
        {
            for (int twice = 0; twice < 2; ++twice) 
            {
                // Choose N is non-depot node
                if (twice > 0)
                    N = si->t2->PFlag ? si->t2 : si->t3;
                else
                    N = si->t1->PFlag ? si->t1 : si->t4;

                if (N->PFlag) {
                    Node *savedN = N;
                    DistanceSum = 0; // length of this route
                    N->PFlag = 0; // mark this route as checked to avoid multiple checks

                    // Forward : loop until meet depot 
                    while ((N = SUC(N))->DepotId == 0) 
                    {
                        N->PFlag = 0;
                        DistanceSum += C(N, SUC(N)) - N->Pi - SUC(N)->Pi;
                    }
                    
                     // Backward : loop until meet depot
                    N = savedN;
                    while ((N = PRED(N))->DepotId == 0)
                    {
                        N->PFlag = 0;
                        DistanceSum += C(N, PRED(N)) - N->Pi - PRED(N)->Pi;
                    }
                    DistanceSum /= Precision;

                    // TODO : update P according to new DistanceSum
                    P = MAX(oldPenaltyMax, DistanceSum);

                    if (P > oldPenaltyMax ||
                         (P == oldPenaltyMax && CurrentGain <= 0))
                    {
                        // no improvement
                        for (SwapRecord *s = si - 1; s >= SwapStack; --s)
                            s->t1->PFlag = s->t2->PFlag = s->t3->PFlag = s->t4->PFlag = 0;
                        return CurrentPenalty;
                    }
                }
            }
        }

        if (!CurrentPenalty)
            return P;
        if (P < oldPenaltyMax ||
            (P == oldPenaltyMax && CurrentGain > 0))
        {
            update_Penalty_MTSP_MINMAX(); //Improved!
            printf("P : %d\n", P);
            printf("CurrentPenalty : %d\n", CurrentPenalty);
            printff("New improved penalty value MIN(CurrentPenalty, P): %d\n", MIN(CurrentPenalty, P));
            return MIN(CurrentPenalty, P); // because now we have a better solution, smaller than the oldPenaltyMax
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
            update_Penalty_MTSP_MINMAX();
        }
        return P;
    }
}

/* Returns 1 if only one route is involved in the current move */
static int setup_Penalty_MTSP_MINMAX()
{
    /*
        Setting up the initial penalty values for the routes involved
    */
    oldPenaltyMax = 0;
    int petalCounter = 0;
    if (CurrentPenalty) // Penalty_MTSP_MINMAX_Old() should be executed before this function
    {
        for (SwapRecord *s = SwapStack + Swaps - 1; s >= SwapStack; --s)
        {
            // If a move has involved the edge of an empty route an additional empty one needs to be counted
            Node *t1 = s->t1, *t2 = s->t2, *t3 = s->t3, *t4 = s->t4;
            // the edges (t1, t2) and (t3, t4) are removed,
            // and the new edges (t1, t3) and (t2, t4) are added to form a new tour.

            // was_empty_route: a route with no customers, just a depot-to-depot link
            if ((!ARE_LINKED(t1, t2) && was_empty_route(t1, t2)) ||
                (!ARE_LINKED(t3, t4) && was_empty_route(t3, t4)))
            {
                ++petalCounter;
            }

            petalCounter += setup_Node_MTSP_MINMAX(t1) + setup_Node_MTSP_MINMAX(t2) +
                            setup_Node_MTSP_MINMAX(t3) + setup_Node_MTSP_MINMAX(t4);
        }
        // Reset petals flags for next petal counting
        for (SwapRecord *s = SwapStack + Swaps - 1; s >= SwapStack; --s)
        {
            Node *t1 = s->t1, *t2 = s->t2, *t3 = s->t3, *t4 = s->t4;
            int d1 = t1->DepotId, d2 = t2->DepotId, d3 = t3->DepotId, d4 = t4->DepotId;

            cava_PetalsData[d1].flag = cava_PetalsData[d2].flag =
                cava_PetalsData[d3].flag = cava_PetalsData[d4].flag = 0;

            t1->PetalId->flag = t2->PetalId->flag = t3->PetalId->flag = t4->PetalId->flag = 0;
        }
        // if (petalCounter == 1)
        //     return 1;
    }
    // mark non-depot nodes involved in the swaps with PFlag = 1,
    // Depot nodes are marked with PFlag = 0 
    for (SwapRecord *s = SwapStack + Swaps - 1; s >= SwapStack; --s)
    {
        s->t1->PFlag = !s->t1->DepotId;
        s->t2->PFlag = !s->t2->DepotId;
        s->t3->PFlag = !s->t3->DepotId;
        s->t4->PFlag = !s->t4->DepotId;
    }
    return petalCounter;
}

static int setup_Node_MTSP_MINMAX(Node *N)
{
    /*
    The role of setup_Node_CVRP() is to ensure that each route's penalty is counted only once
    during the setup phase of the penalty calculation. This helps in accurately computing
    the previous penalty max (oldPenaltyMax) for the routes involved in the current move,
    which is essential for determining if the new solution is an improvement.
    */
    if (!N->PetalId->flag) // if the Node's Route has not been Processed
    {
        oldPenaltyMax = MAX(oldPenaltyMax, N->PetalId->OldPenalty); // update the oldPenaltyMax
        N->PetalId->flag = 1; // Mark Route as Processed
        // returns 1 if the node is not a depot (indicating a valid route), otherwise it returns 0
        return (N->PetalId != cava_PetalsData); // Depots have PetalId_index == 0
    }
    return 0;
}

/* Update route data when a new improving tour is found */
static void update_Penalty_MTSP_MINMAX()
{
    // updates the penalty metadata for each route in the solution
    // It iterates through all routes starting from the depot
    // and updates the OldPenalty and minNode fields in the RouteData structure for each route.
    // not the entire solution / max(all routes)
    int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;
    Node *N = Depot, *NextN;
    RouteData *CurrId;
    GainType Cost;
    int Size;
    int i = 1;
    do
    {
        Size = 0;
        Cost = 0;
        N->PetalId = cava_PetalsData; // depots point to 0 cell
        CurrId = cava_PetalsData + N->DepotId;
        CurrId->OldPenalty = 0;
        
        do {
            ++Size;
            N->PetalId = CurrId;
            NextN = Forward ? SUCC(N) : PREDD(N);
            if (NextN->Id > DimensionSaved)
                NextN = Forward ? SUCC(NextN) : PREDD(NextN);
            Cost += C(N, NextN) - N->Pi - NextN->Pi;
        } while ((N = NextN)->DepotId == 0);
        Cost /= Precision;
        CurrId->OldPenalty = Cost;
        printff("New: route %d : %d\n", i++, Cost);
        CurrId->minNode = Size ? NULL : N; /* Save the adjacent depot to recognize empty routes */
    } while (N != Depot);
}

GainType Penalty_MTSP_MINMAX_Old()
#else
GainType Penalty_MTSP_MINMAX()
#endif
{
    // Forward is true if the next node is not the first node of the next route
    int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;
    static Node *StartRoute = 0;
    Node *N, *NextN, *CurrentRoute;
    GainType Cost, P = MINUS_INFINITY;

    StartRoute = Depot;
    N = StartRoute;
    do {
        Cost = 0;
        CurrentRoute = N;
        do {
            NextN = Forward ? SUCC(N) : PREDD(N);
            if (NextN->Id > DimensionSaved)
                NextN = Forward ? SUCC(NextN) : PREDD(NextN);
            // Cost is the sum of the distances between the nodes in the route
            // minus the Pi values of the nodes
            Cost += C(N, NextN) - N->Pi - NextN->Pi;
        } while ((N = NextN)->DepotId == 0);
        Cost /= Precision;
        if (Cost > P) {
            if (Cost > CurrentPenalty ||
                (Cost == CurrentPenalty && CurrentGain <= 0)) {
                return CurrentPenalty + (CurrentGain > 0);
            }
            P = Cost;
        }
    } while (N != StartRoute);
    printff("Old: all routes : %d\n", P);
    return P;
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
