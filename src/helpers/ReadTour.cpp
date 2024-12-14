#include "ReadTour.h"

void Read_TOUR_SECTION(FILE ** File)
{
    Node *First = 0, *Last = 0, *N, *Na;
    int i, k;

    if (TraceLevel >= 1) {
        printff("Reading ");
        if (File == &InitialTourFile)
            printff("INITIAL_TOUR_FILE: \"%s\" ... ", InitialTourFileName);
        else if (File == &InputTourFile)
            printff("INPUT_TOUR_FILE: \"%s\" ... ", InputTourFileName);
        else if (File == &SubproblemTourFile)
            printff("SUBPROBLEM_TOUR_FILE: \"%s\" ... ",
                    SubproblemTourFileName);
        else
            for (i = 0; i < MergeTourFiles; i++)
                if (File == &MergeTourFile[i])
                    printff("MERGE_TOUR_FILE: \"%s\" ... ",
                            MergeTourFileName[i]);
    }
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
    if (!fscanint(*File, &i))
        i = -1;
    else if (i == 0) {
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
            if (File == &InitialTourFile) {
                if (!Na)
                    Last->InitialSuc = N;
                else {
                    Last->InitialSuc = Na;
                    Na->InitialSuc = N;
                }
            } else if (File == &InputTourFile) {
                if (!Na)
                    Last->InputSuc = N;
                else {
                    Last->InputSuc = Na;
                    Na->InputSuc = N;
                }
            } else if (File == &SubproblemTourFile) {
                if (!Na)
                    (Last->SubproblemSuc = N)->SubproblemPred = Last;
                else {
                    (Last->SubproblemSuc = Na)->SubproblemPred = Last;
                    (Na->SubproblemSuc = N)->SubproblemPred = Na;
                }
            } else {
                for (i = 0; i < MergeTourFiles; i++) {
                    if (File == &MergeTourFile[i]) {
                        if (!Na) {
                            Last->MergeSuc[i] = N;
                            if (i == 0)
                                N->MergePred = Last;
                        } else {
                            Last->MergeSuc[i] = Na;
                            Na->MergeSuc[i] = N;
                            if (i == 0) {
                                Na->MergePred = Last;
                                N->MergePred = Na;
                            }
                        }
                    }
                }
            }
            Last = N;
        }
        if (k < Dimension) {
            fscanint(*File, &i);
            if (b)
                if (i >= 0)
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
    if (File == &SubproblemTourFile) {
        do {
            if (N->FixedTo1 &&
                N->SubproblemPred != N->FixedTo1
                && N->SubproblemSuc != N->FixedTo1)
                eprintf("Fixed edge (%d, %d) "
                        "does not belong to subproblem tour", N->Id,
                        N->FixedTo1->Id);
            if (N->FixedTo2 && N->SubproblemPred != N->FixedTo2
                && N->SubproblemSuc != N->FixedTo2)
                eprintf("Fixed edge (%d, %d) "
                        "does not belong to subproblem tour", N->Id,
                        N->FixedTo2->Id);
        } while ((N = N->Suc) != FirstNode);
    }
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