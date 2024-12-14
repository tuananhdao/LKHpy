#include "ReadTour.h"

void Read_TOUR_SECTION(py::array_t<int> initial_tour)
{
    auto tour_data = initial_tour.unchecked<1>(); // Access the data
    if (tour_data.size() == 0) {
        printff("Initial tour is empty.\n");
        return;
    }

    Node *First = 0, *Last = 0, *N, *Na;
    int i, k;

    if (!FirstNode)
        CreateNodes();
    N = FirstNode;
    do
        N->V = 0;
    while ((N = N->Suc) != FirstNode);
    if (ProblemType == HPP)
        Dimension--;
    if (Asymmetric)
        Dimension = DimensionSaved;
    int b = 0;
    i = (0 < tour_data.size()) ? tour_data(0) : -1;
    if (i == 0) {
        b = 1;
        i++;
    }
    for (k = 0; k <= Dimension && i != -1; k++) {
        if (i <= 0 || i > Dimension)
            eprintf("TOUR_SECTION: Node number out of range: %d", i);
        N = &NodeSet[i];
        if (N->V == 1 && k != Dimension)
            eprintf("TOUR_SECTION: Node number occurs twice: %d", N->Id);
        N->V = 1;
        if (k == 0)
            First = Last = N;
        else {
            if (Asymmetric) {
                Na = N + Dimension;
                Na->V = 1;
            } else
                Na = 0;
            if (!Na)
                Last->InitialSuc = N;
            else {
                Last->InitialSuc = Na;
                Na->InitialSuc = N;
            }
            Last = N;
        }
        if (k < Dimension - 1) {
            i = ((k + 1) < tour_data.size()) ? tour_data(k + 1) : -1;
            if (b && i >= 0)
                i++;
        }
        if (k == Dimension - 1)
            i = First->Id;
    }
    N = FirstNode;
    do {
        if (!N->V)
            eprintf("TOUR_SECTION: Node is missing: %d", N->Id);
    } while ((N = N->Suc) != FirstNode);
    if (ProblemType == HPP)
        Dimension++;
    if (Asymmetric)
        Dimension *= 2;
    if (TraceLevel >= 1)
        printff("done\n");
}

static void CreateNodes()
{
    Node *Prev = 0, *N = 0;
    int i;

    if (Dimension <= 0)
        eprintf("DIMENSION is not positive (or not specified)");
    if (Asymmetric) {
        Dim = DimensionSaved;
        DimensionSaved = Dimension + Salesmen - 1;
        Dimension = 2 * DimensionSaved;
    } else if (ProblemType == HPP) {
        Dimension++;
        if (Dimension > MaxMatrixDimension)
            eprintf("DIMENSION too large in HPP problem");
    }
    if (ProblemType == GCTSP)
        NodeSet = (Node *) calloc(Dimension + 2, sizeof(Node));
    else
        NodeSet = (Node *) calloc(Dimension + 1, sizeof(Node));
    for (i = 1; i <= Dimension; i++, Prev = N) {
        N = &NodeSet[i];
        if (i == 1)
            FirstNode = N;
        else
            Link(Prev, N);
        N->Id = N->OriginalId = i;
        if (MergeTourFiles >= 1)
            N->MergeSuc = (Node **) calloc(MergeTourFiles, sizeof(Node *));
        N->Earliest = 0;
        N->Latest = INT_MAX;
    }
    Link(N, FirstNode);
}