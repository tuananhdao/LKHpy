#include "LKH.h"
#include "Segment.h"

// #define REDUNDANT_CHECK /* ONLY DEBUG: checks old and new and assert they are the same. */
#define printffff if (0) printff // when investigating speedup, set to if (1)

#ifdef CAVA_PENALTY
static GainType oldPenaltyMax;

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
GainType Penalty_MTSP_MINMAX_Old(void);
static GainType update_Penalty_MTSP_MINMAX(void);
static GainType update_Penalty_MTSP_MINMAX_Old(void);
static void setup_Node_MTSP_MINMAX(Node *);
static void setup_Penalty_MTSP_MINMAX(void);
static GainType calculate_DistanceSum(Node *initN, int Forward);

int SwapCaseCount = 0;
#define printfff if (SwapCaseCount == -1) printff
#endif

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

#ifdef REDUNDANT_CHECK
static GainType Penalty_MTSP_MINMAX_();
GainType Penalty_MTSP_MINMAX()
{
    assert(CurrentPenalty >= 0);
    GainType P2 = Penalty_MTSP_MINMAX_Old();
    GainType P1 = Penalty_MTSP_MINMAX_();

    int accepted1 = P1 < CurrentPenalty || (P1 == CurrentPenalty && CurrentGain > 0);
    int accepted2 = P2 < CurrentPenalty || (P2 == CurrentPenalty && CurrentGain > 0);
    // if (P1 != P2) {
    //     printffff("-- P1 (new): %d\n", P1);
    //     printffff("-- P2 (old): %d\n", P2);
    // }
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
        // printff("%lld\n", SwapCaseCount);
        // SwapCaseCount++;
        //printfff("\nSwaps LOOP!! Swaps: %d\n", Swaps);
        setup_Penalty_MTSP_MINMAX();

        GainType DistanceSum;
        GainType MaxOldPenaltyInSwaps = 0;
        Node *N;

        // Early exit 1
        for (int i = 1; i < Salesmen + 1; i++)
        {
            if (cava_PetalsData[i].flag)
            {
                MaxOldPenaltyInSwaps = MAX(MaxOldPenaltyInSwaps, cava_PetalsData[i].OldPenalty);
                cava_PetalsData[i].flag = 0;
            }
        }
        if (MaxOldPenaltyInSwaps < oldPenaltyMax)
        {
            printffff("MaxOldPenaltyInSwaps %d < oldPenaltyMax %d. Skipped.\n", MaxOldPenaltyInSwaps, oldPenaltyMax);
            return CurrentPenalty;
        }

        // Early exit 2
        if (oldPenaltyMax < CurrentPenalty)
        {
            // If the max route is not changed
            // the penalty will not change
            // because oldPenaltyMax was improved (smaller)
            printffff("oldPenaltyMax < CurrentPenalty. Skipped.\n");
            for (SwapRecord *si = SwapStack + Swaps - 1; si >= SwapStack; --si)
            {
                for (int twice = 0; twice < 2; ++twice)
                {
                    if (twice > 0)
                        N = si->t2->PFlag ? si->t2 : si->t3;
                    else
                        N = si->t1->PFlag ? si->t1 : si->t4;
                    if (N->PFlag)
                    {
                        // Calculate the penalty for the route
                        DistanceSum = calculate_DistanceSum(N, 1) + calculate_DistanceSum(N, 0);

                        if (DistanceSum > CurrentPenalty)
                            return CurrentPenalty + (CurrentGain > 0);
                    }
                }
            }
            return CurrentPenalty;
        }

        // Main improved loop
        for (SwapRecord *si = SwapStack + Swaps - 1; si >= SwapStack; --si)
        {
            for (int twice = 0; twice < 2; ++twice)
            {
                if (twice > 0)
                    N = si->t2->PFlag ? si->t2 : si->t3;
                else
                    N = si->t1->PFlag ? si->t1 : si->t4;
                if (N->PFlag)
                {
                    // Calculate the penalty for the route
                    DistanceSum = calculate_DistanceSum(N, 1) + calculate_DistanceSum(N, 0);

                    //printfff("DistanceSum: %d (or %d) -> %d\n", savedN->PetalId->OldPenalty, N->PetalId->OldPenalty, DistanceSum);
                    P = MAX(P, DistanceSum);

                    if (P > oldPenaltyMax ||
                         (P == oldPenaltyMax && CurrentGain <= 0))
                    {
                        for (SwapRecord *s = si - 1; s >= SwapStack; --s)
                            s->t1->PFlag = s->t2->PFlag = s->t3->PFlag = s->t4->PFlag = 0;
                        printffff("P: %d > oldPenaltyMax: %d. Skipped.\n", P, oldPenaltyMax);
                        return CurrentPenalty + (CurrentGain > 0);
                    }
                }
            }
        }

        if (!CurrentPenalty)
            return P;

        printffff("Improved!\n");
        printffff("-- P: %d\n", P);
        printffff("-- oldPenaltyMax: %d\n", oldPenaltyMax);
        printffff("-- CurrentPenalty: %d\n", CurrentPenalty);
        return update_Penalty_MTSP_MINMAX(); //Improved!
    }
    else
    {
        printffff("Using the old penalty function.\n");
        if (!cava_PetalsData)
            cava_PetalsData = (RouteData *)calloc(Salesmen + 1, sizeof(RouteData));
        return update_Penalty_MTSP_MINMAX_Old();
    }
}

static GainType calculate_DistanceSum(Node *initN, int Forward)
{
    // TODO: Cache GainType *SUC_N_SUM = calloc(Dimension, sizeof(GainType));
    //         and GainType *PRED_N_SUM = calloc(Dimension, sizeof(GainType));
    // This function directly returns Forward ? SUC_N_SUM[initN->Id] : PRED_N_SUM[initN->Id];
    // O(1) instead of O(n) for each call
    GainType DistanceSum = 0;
    Node *N = initN, *NextN;
    N->PFlag = 0;

    //Forward
    while (N->DepotId == 0)
    {
        NextN = Forward ? SUCC(N) : PREDD(N);
        DistanceSum += _Penalty(N, NextN);
        N = NextN;
        N->PFlag = 0;
    }
    DistanceSum /= Precision;
    return DistanceSum;
}

/* Returns 1 if only one route is involved in the current move */
static void setup_Penalty_MTSP_MINMAX()
{
    /*
        Setting up the initial penalty values for the routes involved
    */
    oldPenaltyMax = 0;
    if (CurrentPenalty) // Penalty_MTSP_MINMAX_Old() should be executed before this function
    {
        for (SwapRecord *s = SwapStack + Swaps - 1; s >= SwapStack; --s)
        {
            // If a move has involved the edge of an empty route an additional empty one needs to be counted
            Node *t1 = s->t1, *t2 = s->t2, *t3 = s->t3, *t4 = s->t4;
            // the edges (t1, t2) and (t3, t4) are removed,
            // and the new edges (t1, t4) and (t2, t3) are added to form a new tour.

            setup_Node_MTSP_MINMAX(t1);
            setup_Node_MTSP_MINMAX(t2);
            setup_Node_MTSP_MINMAX(t3);
            setup_Node_MTSP_MINMAX(t4);
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
    }
    // mark non-depot nodes involved in the swaps with PFlag = 1,
    // Depot nodes are marked with PFlag = 0 
    for (SwapRecord *s = SwapStack + Swaps - 1; s >= SwapStack; --s)
    {
        s->t1->PFlag = !s->t1->DepotId;
        s->t2->PFlag = !s->t2->DepotId;
        s->t3->PFlag = !s->t3->DepotId;
        s->t4->PFlag = !s->t4->DepotId;

        // mark the involved routes with flag = 1 for the early exit MaxOldPenaltyInSwaps < oldPenaltyMax
        s->t1->PetalId->flag = s->t2->PetalId->flag = s->t3->PetalId->flag = s->t4->PetalId->flag = 1;
    }
}

static void setup_Node_MTSP_MINMAX(Node *N)
{
    /*
    The role of setup_Node_CVRP() is to ensure that each route's penalty is counted only once
    during the setup phase of the penalty calculation. This helps in accurately computing
    the previous penalty sum (oldPenaltyMax) for the routes involved in the current move,
    which is essential for determining if the new solution is an improvement.
    */
    if (!N->PetalId->flag) // check if the Node's Route has been Processed
    {
        oldPenaltyMax = MAX(oldPenaltyMax, N->PetalId->OldPenalty); // update the oldPenaltyMax
        //printfff("Route involved: %d : %d\n", N->PetalId - cava_PetalsData, N->PetalId->OldPenalty);
        N->PetalId->flag = 1; // Mark Route as Processed
    }
}

/* Update route data when a new improving tour is found */
static GainType update_Penalty_MTSP_MINMAX()
{
    // updates the penalty metadata for each route in the solution
    // It iterates through all routes starting from the depot
    // and updates the OldPenalty in the RouteData structure for each route.
    // not the entire solution / max(all routes)
    int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;
    Node *N = Depot, *NextN;
    RouteData *CurrId;
    GainType Cost;
    GainType MaxCost = 0;
    int i = 1;
    // printfff("Update Penalty_MTSP_MINMAX (expensive func)\n");
    do
    {
        Cost = 0;
        N->PetalId = cava_PetalsData; // depots point to 0 cell
        CurrId = cava_PetalsData + N->DepotId;
        CurrId->OldPenalty = 0;
        
        do {
            N->PetalId = CurrId;
            NextN = Forward ? SUCC(N) : PREDD(N);
            Cost += _Penalty(N, NextN);
            // printfff("%d -> %d: %d\n", N->Id, NextN->Id, (C(N, NextN) - N->Pi - NextN->Pi)/Precision);
        } while ((N = NextN)->DepotId == 0);
        Cost /= Precision;
        CurrId->OldPenalty = Cost;
        MaxCost = MAX(MaxCost, Cost);
        // printfff("-- New: route %d : %d\n", i++, Cost);
    } while (N != Depot);
    return MaxCost;
}

static GainType update_Penalty_MTSP_MINMAX_Old()
{
    // Forward is true if the next node is not the first node of the next route
    int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;
    Node *N = Depot, *NextN;
    GainType Cost, MaxCost = MINUS_INFINITY;
    RouteData *CurrId;
    do {
        Cost = 0;
        N->PetalId = cava_PetalsData; // depots point to 0 cell
        CurrId = cava_PetalsData + N->DepotId;
        CurrId->OldPenalty = 0;
        do {
            N->PetalId = CurrId;
            NextN = Forward ? SUCC(N) : PREDD(N);
            if (NextN->Id > DimensionSaved)
                NextN = Forward ? SUCC(NextN) : PREDD(NextN);
            Cost += _Penalty(N, NextN);
        } while ((N = NextN)->DepotId == 0);
        Cost /= Precision;
        CurrId->OldPenalty = Cost;
        if (Cost > MaxCost) {
            if (Cost > CurrentPenalty ||
                (Cost == CurrentPenalty && CurrentGain <= 0)) {
                MaxCost = CurrentPenalty + (CurrentGain > 0);
                // break; // maybe wrong?
            }
            else
            {
                MaxCost = Cost;
            }
        }
    } while (N != Depot);
    return MaxCost;
}

GainType Penalty_MTSP_MINMAX_Old()
#else
GainType Penalty_MTSP_MINMAX()
#endif
{
    // Forward is true if the next node is not the first node of the next route
    int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;
    static Node *StartRoute = 0;
    Node *N = Depot, *NextN;
    GainType Cost, MaxCost = MINUS_INFINITY;

    do {
        Cost = 0;
        do {
            NextN = Forward ? SUCC(N) : PREDD(N);
            if (NextN->Id > DimensionSaved)
                NextN = Forward ? SUCC(NextN) : PREDD(NextN);
            // Cost is the sum of the distances between the nodes in the route
            // minus the Pi values of the nodes
            Cost += _Penalty(N, NextN);
        } while ((N = NextN)->DepotId == 0);
        Cost /= Precision;
        if (Cost > MaxCost) {
            if (Cost > CurrentPenalty ||
                (Cost == CurrentPenalty && CurrentGain <= 0)) {
                return CurrentPenalty + (CurrentGain > 0);
            }
            MaxCost = Cost;
        }
    } while (N != Depot);
    // printfff("_Old() all routes : %d\n", P);
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
