#include "LKH.h"
#include "Segment.h"

// #define REDUNDANT_CHECK /* ONLY DEBUG: checks old and new and assert they are the same. */
#define printffff if (0) printff // when investigating speedup, set to if (1)

#ifdef CAVA_PENALTY
static GainType oldPenaltyMax;

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
GainType Penalty_MTSP_MINMAX_Old(void);
GainType Penalty_MTSP_MINMAX_Old_And_Update(void);
static GainType update_Penalty_MTSP_MINMAX(void);
static void setup_Node_MTSP_MINMAX(Node *);
static void setup_Penalty_MTSP_MINMAX(void);
static GainType calculate_DistanceSum(Node *initN, int Forward);
static GainType get_max_cost();

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
    // prevent segmenation fault
    GainType P2 = Penalty_MTSP_MINMAX_Old();
    GainType P1 = Penalty_MTSP_MINMAX_();

    int accepted1 = P1 < CurrentPenalty || (P1 == CurrentPenalty && CurrentGain > 0);
    int accepted2 = P2 < CurrentPenalty || (P2 == CurrentPenalty && CurrentGain > 0);
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
        setup_Penalty_MTSP_MINMAX();

        GainType DistanceSum;
        GainType MaxOldPenaltyInSwaps = 0;
        Node *N;

        // Early exit 2
        if (oldPenaltyMax < CurrentPenalty)
        {
            // If the max route is not changed
            // the penalty will not change
            // because oldPenaltyMax was improved (smaller)
            // printffff("oldPenaltyMax < CurrentPenalty. Skipped.\n");
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
                    // For early exit 1
                    MaxOldPenaltyInSwaps = MAX(MaxOldPenaltyInSwaps, N->PetalId->OldPenalty);
                    MaxOldPenaltyInSwaps = MAX(MaxOldPenaltyInSwaps, SUCC(N)->PetalId->OldPenalty);
                    MaxOldPenaltyInSwaps = MAX(MaxOldPenaltyInSwaps, PREDD(N)->PetalId->OldPenalty);
                    // Calculate the penalty for the route
                    DistanceSum = calculate_DistanceSum(N, 1) + calculate_DistanceSum(N, 0);
                    // printff("N, PREDD(N), SUCC(N): %d, %d, %d\n", N->Id, PREDD(N)->Id, SUCC(N)->Id);

                    //printfff("DistanceSum: %d (or %d) -> %d\n", savedN->PetalId->OldPenalty, N->PetalId->OldPenalty, DistanceSum);
                    P = MAX(P, DistanceSum);

                    if (P > oldPenaltyMax ||
                         (P == oldPenaltyMax && CurrentGain <= 0))
                    {
                        for (SwapRecord *s = si - 1; s >= SwapStack; --s)
                            s->t1->PFlag = s->t2->PFlag = s->t3->PFlag = s->t4->PFlag = 0;
                        // printffff("P: %d > oldPenaltyMax: %d. Skipped.\n", P, oldPenaltyMax);
                        return CurrentPenalty + (CurrentGain > 0);
                    }
                }
            }
        }

        if (MaxOldPenaltyInSwaps < oldPenaltyMax)
        {
            // printffff("MaxOldPenaltyInSwaps %d < oldPenaltyMax %d. Skipped.\n", MaxOldPenaltyInSwaps, oldPenaltyMax);
            return CurrentPenalty;
        }

        P = get_max_cost();
        if (P == CurrentPenalty && CurrentGain < 0)
        {
            return CurrentPenalty;
        }
        else
        {
            return update_Penalty_MTSP_MINMAX();
        }
    }
    else
    {
        //printffff("Using the old penalty function.\n");
        if (!cava_PetalsData)
            cava_PetalsData = (RouteData *)calloc(Salesmen + 1, sizeof(RouteData));
        return Penalty_MTSP_MINMAX_Old_And_Update();
    }
}

static GainType calculate_DistanceSum(Node *initN, int Forward)
{
    GainType DistanceSum = 0;
    Node *N = initN, *NextN;
    N->PFlag = 0;

    //Forward
    while (N->DepotId == 0)
    {
        NextN = Forward ? SUCC(N) : PREDD(N);
        GainType HashMapCost = MinNodeHashSearch(MinNodeHTable, N->Id, NextN->PetalRank);
        if (HashMapCost > 0 && (int)(N->PetalId - cava_PetalsData) == (int)(NextN->PetalId - cava_PetalsData))
        {
            DistanceSum += HashMapCost;
            break;
        }
        DistanceSum += MTSP_Penalty(N, NextN);
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
    int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;
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
    }
    MinNodeHashInitialize(MinNodeHTable);
    for (int i = 1; i < Salesmen + 1; i++)
    {
        RouteData *Petal = cava_PetalsData + i;
        Node *N = Petal->minNode;
        if (N == NULL)
            continue;
        Node *PrevN = Forward ? SUCC(N) : PREDD(N);
        int PrevRank = Forward ? N->PetalRank - 1 : N->PetalRank + 1;
        if (N->DepotId != 0 || PrevN == NULL || PrevN->DepotId != 0)
            continue;
        MinNodeHashInsert(MinNodeHTable, N->Id, PrevRank, N->prevCostSum);

        N = Petal->maxNode; // not null
        PrevN = Forward ? PREDD(N) : SUCC(N);
        PrevRank = Forward ? N->PetalRank + 1 : N->PetalRank - 1;
        if (N->DepotId != 0 || PrevN == NULL || PrevN->DepotId != 0)
            continue;
        MinNodeHashInsert(MinNodeHTable, N->Id, PrevRank, N->PetalId->OldPenalty * Precision - N->prevCostSum);
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

    if (!N->PetalId->minNode || N->PetalId->minNode->PetalRank > N->PetalRank)
    {
        N->PetalId->minNode = N;
    }
    if (!N->PetalId->maxNode || N->PetalId->maxNode->PetalRank < N->PetalRank)
    {
        N->PetalId->maxNode = N;
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
    // printff("Round %d update_Penalty_MTSP_MINMAX\n", SwapCaseCount);
    // printfff("Update Penalty_MTSP_MINMAX (expensive func)\n");
    do
    {
        Cost = 0;
        // N->PetalId = cava_PetalsData; // depots point to 0 cell
        CurrId = cava_PetalsData + N->DepotId;
        CurrId->OldPenalty = 0;
        int PetalRank = 0;
        do {
            N->PetalRank = PetalRank++;
            N->prevCostSum = Cost;
            N->PetalId = CurrId;
            NextN = Forward ? SUCC(N) : PREDD(N);
            if (NextN->Id > DimensionSaved)
                NextN = Forward ? SUCC(NextN) : PREDD(NextN);
            Cost += MTSP_Penalty(N, NextN);
            // printfff("%d -> %d: %d\n", N->Id, NextN->Id, (C(N, NextN) - N->Pi - NextN->Pi)/Precision);
        } while ((N = NextN)->DepotId == 0);
        Cost /= Precision;
        CurrId->OldPenalty = Cost;
        MaxCost = MAX(MaxCost, Cost);
        // printfff("-- New: route %d : %d\n", i++, Cost);
    } while (N != Depot);

    for (int i = 1; i < Salesmen + 1; i++)
    {
        cava_PetalsData[i].minNode = NULL;
        cava_PetalsData[i].maxNode = NULL;
    }

    return MaxCost;
}

static GainType get_max_cost()
{
    int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;
    Node *N = Depot, *NextN;
    GainType Cost;
    GainType MaxCost = 0;
    do
    {
        Cost = 0;
        do {
            NextN = Forward ? SUCC(N) : PREDD(N);
            if (NextN->Id > DimensionSaved)
                NextN = Forward ? SUCC(NextN) : PREDD(NextN);
            Cost += MTSP_Penalty(N, NextN);
        } while ((N = NextN)->DepotId == 0);
        Cost /= Precision;
        MaxCost = MAX(MaxCost, Cost);
    } while (N != Depot);

    return MaxCost;
}

GainType Penalty_MTSP_MINMAX_Old_And_Update()
{
    // Forward is true if the next node is not the first node of the next route
    int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;
    Node *N = Depot, *NextN;
    GainType Cost, MaxCost = MINUS_INFINITY;
    RouteData *CurrId;
    // printff("Round %d Penalty_MTSP_MINMAX_Old\n", SwapCaseCount);
    do {
        Cost = 0;
        // N->PetalId = cava_PetalsData; // depots point to 0 cell
        CurrId = cava_PetalsData + N->DepotId;
        CurrId->OldPenalty = 0;
        int PetalRank = 0;
        do {
            N->PetalRank = PetalRank++;
            N->prevCostSum = Cost;
            N->PetalId = CurrId;
            NextN = Forward ? SUCC(N) : PREDD(N);
            if (NextN->Id > DimensionSaved)
                NextN = Forward ? SUCC(NextN) : PREDD(NextN);
            Cost += MTSP_Penalty(N, NextN);
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

    for (int i = 1; i < Salesmen + 1; i++)
    {
        cava_PetalsData[i].minNode = NULL;
        cava_PetalsData[i].maxNode = NULL;
    }

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
            Cost += MTSP_Penalty(N, NextN);
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
