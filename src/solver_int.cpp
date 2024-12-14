#include "solver_int.h"

static void Read_TOUR_SECTION(FILE ** File);
static void CreateNodes(void);

// Function to accept a 2D NumPy array
py::array_t<int> solve_int(py::str arrayType, py::array_t<int> array, py::dict params) {
    // Ensure the input is a 2D array
    if (array.ndim() != 2) {
        throw std::runtime_error("Input should be a 2D NumPy array");
    }

    GainType Cost, OldOptimum;
    double Time, LastTime;
    Node *N;
    int i;

    ParameterFileName = "Dummy";
    // ReadParameters();
    ReadParametersFromDictionary(params);
    
    StartTime = LastTime = GetTime();
    MaxMatrixDimension = 20000;
    MergeWithTour =
        Recombination == GPX2 ? MergeWithTourGPX2 :
        Recombination == CLARIST ? MergeWithTourCLARIST :
                                   MergeWithTourIPT;

    // ReadProblem();
    if (std::string(arrayType) == "cost_matrix") {
        ReadMatrix(array);
    }
    else if (std::string(arrayType) == "euclid") {
        ReadXY_int(arrayType, array);
    }
    else {
        throw std::runtime_error("Invalid array type");
    } 

    FILE *file = fopen("/root/LKHpy/src/sample.tour", "r");
    if (file == NULL) {
        perror("Error opening file");
    }
    Read_TOUR_SECTION(&file);

    if (SubproblemSize > 0) {
        if (DelaunayPartitioning)
            SolveDelaunaySubproblems();
        else if (KarpPartitioning)
            SolveKarpSubproblems();
        else if (KCenterPartitioning)
            SolveKCenterSubproblems();
        else if (KMeansPartitioning)
            SolveKMeansSubproblems();
        else if (RohePartitioning)
            SolveRoheSubproblems();
        else if (MoorePartitioning || SierpinskiPartitioning)
            SolveSFCSubproblems();
        else
            SolveTourSegmentSubproblems();
        return GetOutputTour(BestTour);
    }
    AllocateStructures();
    if (ProblemType == TSPTW)
        TSPTW_Reduce();
    if (ProblemType == VRPB || ProblemType == VRPBTW)
        VRPB_Reduce();
    if (ProblemType == PDPTW)
        PDPTW_Reduce();
    CreateCandidateSet();
    InitializeStatistics();

    if (Norm != 0 || Penalty) {
        Norm = 9999;
        BestCost = PLUS_INFINITY;
        BestPenalty = CurrentPenalty = PLUS_INFINITY;
    } else {
        /* The ascent has solved the problem! */
        Optimum = BestCost = (GainType) LowerBound;
        UpdateStatistics(Optimum, GetTime() - LastTime);
        RecordBetterTour();
        RecordBestTour();
        CurrentPenalty = PLUS_INFINITY;
        BestPenalty = CurrentPenalty = Penalty ? Penalty() : 0;
        // WriteTour(OutputTourFileName, BestTour, BestCost);
        // WriteTour(TourFileName, BestTour, BestCost);
        Runs = 0;
    }

    /* Find a specified number (Runs) of local optima */

    for (Run = 1; Run <= Runs; Run++) {
        // LKHpy: handle KeyboardInterrupt ///////////////////////////////////
        if (PyErr_CheckSignals() != 0)
                throw py::error_already_set();
        //////////////////////////////////////////////////////////////////////
        LastTime = GetTime();
        if (LastTime - StartTime >= TotalTimeLimit) {
            if (TraceLevel >= 1)
                printff("*** Time limit exceeded ***\n");
            Run--;
            break;
        }
        Cost = FindTour();      /* using the Lin-Kernighan heuristic */
        if (MaxPopulationSize > 1 && !TSPTW_Makespan) {
            /* Genetic algorithm */
            int i;
            for (i = 0; i < PopulationSize; i++) {
                GainType OldPenalty = CurrentPenalty;
                GainType OldCost = Cost;
                Cost = MergeTourWithIndividual(i);
                if (TraceLevel >= 1 &&
                    (CurrentPenalty < OldPenalty ||
                     (CurrentPenalty == OldPenalty && Cost < OldCost))) {
                    if (CurrentPenalty)
                        printff("  Merged with %d: Cost = " GainFormat "_"
                                GainFormat, i + 1, CurrentPenalty, Cost);
                    else
                        printff("  Merged with %d: Cost = " GainFormat,
                                i + 1, Cost);
                    if (Optimum != MINUS_INFINITY && Optimum != 0) {
                        if (!Penalty ||
                            (ProblemType != CCVRP &&
                             ProblemType != CBTSP &&
                             ProblemType != CBnTSP &&
                             ProblemType != KTSP &&
                             ProblemType != MLP &&
                             ProblemType != PTSP &&
                             ProblemType != TRP &&
                             MTSPObjective != MINMAX &&
                             MTSPObjective != MINMAX_SIZE))
                            printff(", Gap = %0.4f%%",
                                    100.0 * (Cost - Optimum) / Optimum);
                        else
                            printff(", Gap = %0.4f%%",
                                    100.0 * (CurrentPenalty - Optimum) /
                                    Optimum);
                    }
                    printff("\n");
                }
            }
            if (!HasFitness(CurrentPenalty, Cost)) {
                if (PopulationSize < MaxPopulationSize) {
                    AddToPopulation(CurrentPenalty, Cost);
                    if (TraceLevel >= 1)
                        PrintPopulation();
                } else if (SmallerFitness(CurrentPenalty, Cost,
                                          PopulationSize - 1)) {
                    i = ReplacementIndividual(CurrentPenalty, Cost);
                    ReplaceIndividualWithTour(i, CurrentPenalty, Cost);
                    if (TraceLevel >= 1)
                        PrintPopulation();
                }
            }
        } else if (Run > 1 && !TSPTW_Makespan)
            Cost = MergeTourWithBestTour();
        if (CurrentPenalty < BestPenalty ||
            (CurrentPenalty == BestPenalty && Cost < BestCost)) {
            BestPenalty = CurrentPenalty;
            BestCost = Cost;
            RecordBetterTour();
            RecordBestTour();
            // WriteTour(TourFileName, BestTour, BestCost);
        }
        OldOptimum = Optimum;
        if (!Penalty ||
            (ProblemType != CCVRP &&
             ProblemType != CBTSP &&
             ProblemType != CBnTSP &&
             ProblemType != GCTSP &&
             ProblemType != CCCTSP &&
             ProblemType != KTSP &&
             ProblemType != MLP &&
             ProblemType != PTSP &&
             ProblemType != TRP &&
             Penalty != Penalty_MTSP_MINMAX &&
             Penalty != Penalty_MTSP_MINMAX_SIZE)) {
            if (CurrentPenalty == 0 && Cost < Optimum)
                Optimum = Cost;
        } else if (CurrentPenalty < Optimum)
            Optimum = CurrentPenalty;
        if (Optimum < OldOptimum) {
            printff("*** New OPTIMUM = " GainFormat " ***\n", Optimum);
            if (FirstNode->InputSuc) {
                Node *N = FirstNode;
                while ((N = N->InputSuc = N->Suc) != FirstNode);
            }
        }
        Time = fabs(GetTime() - LastTime);
        UpdateStatistics(Cost, Time);
        if (TraceLevel >= 1 && Cost != PLUS_INFINITY) {
            printff("Run %d: ", Run);
            StatusReport(Cost, LastTime, "");
            printff("\n");
        }
        if (StopAtOptimum && MaxPopulationSize >= 1) {
            if (ProblemType != CCVRP &&
                ProblemType != TRP &&
                ProblemType != CBTSP &&
                ProblemType != CBnTSP &&
                ProblemType != GCTSP &&
                ProblemType != CCCTSP &&
                ProblemType != KTSP &&
                ProblemType != MLP &&
                ProblemType != PTSP &&
                MTSPObjective != MINMAX &&
                MTSPObjective != MINMAX_SIZE ?
                CurrentPenalty == 0 && Cost == Optimum :
                CurrentPenalty == Optimum) {
                Runs = Run;
                break;
            }
        }
        IsChild = 0;
        if (PopulationSize >= 2 &&
            (PopulationSize == MaxPopulationSize ||
             Run >= 2 * MaxPopulationSize) && Run < Runs) {
            Node *N;
            int Parent1, Parent2;
            Parent1 = LinearSelection(PopulationSize, 1.25);
            do
                Parent2 = LinearSelection(PopulationSize, 1.25);
            while (Parent2 == Parent1);
            ApplyCrossover(Parent1, Parent2);
            IsChild = 1;
            N = FirstNode;
            do {
                if (ProblemType != HCP && ProblemType != HPP) {
                    int d = C(N, N->Suc);
                    AddCandidate(N, N->Suc, d, INT_MAX);
                    AddCandidate(N->Suc, N, d, INT_MAX);
                }
                N = N->InitialSuc = N->Suc;
            } while (N != FirstNode);
        }
        SRandom(++Seed);
    }
    PrintStatistics();
    if (Salesmen > 1) {
        if (Dimension == DimensionSaved) {
            for (i = 1; i <= Dimension; i++) {
                N = &NodeSet[BestTour[i - 1]];
                (N->Suc = &NodeSet[BestTour[i]])->Pred = N;
            }
        } else {
            for (i = 1; i <= DimensionSaved; i++) {
                Node *N1 = &NodeSet[BestTour[i - 1]];
                Node *N2 = &NodeSet[BestTour[i]];
                Node *M1 = &NodeSet[N1->Id + DimensionSaved];
                Node *M2 = &NodeSet[N2->Id + DimensionSaved];
                (M1->Suc = N1)->Pred = M1;
                (N1->Suc = M2)->Pred = N1;
                (M2->Suc = N2)->Pred = M2;
            }
        }
        CurrentPenalty = BestPenalty;
        MTSP_Report(BestPenalty, BestCost);
        //MTSP_WriteSolution(MTSPSolutionFileName, BestPenalty, BestCost);
    }
    //SINTEF_WriteSolution(SINTEFSolutionFileName, BestCost);
    if (ProblemType == ACVRP ||
        ProblemType == BWTSP ||
        ProblemType == CCVRP ||
        ProblemType == CTSP ||
        ProblemType == CVRP ||
        ProblemType == CVRPTW ||
        ProblemType == GCTSP ||
        ProblemType == CCCTSP ||
        ProblemType == MLP ||
        ProblemType == M_PDTSP ||
        ProblemType == M1_PDTSP ||
        MTSPObjective != -1 ||
        ProblemType == ONE_PDTSP ||
        ProblemType == OVRP ||
        ProblemType == PDTSP ||
        ProblemType == PDTSPL ||
        ProblemType == PDPTW ||
        ProblemType == PTSP ||
        ProblemType == RCTVRP ||
        ProblemType == RCTVRPTW ||
        ProblemType == SOP ||
        ProblemType == TRP ||
        ProblemType == TSPTW ||
        ProblemType == VRPB ||
        ProblemType == VRPBTW || ProblemType == VRPPD) {
        printff("Best %s solution:\n", Type);
        CurrentPenalty = BestPenalty;
        SOP_Report(BestCost);
    }
    printff("\n");
    return GetOutputTour(BestTour);
}


static void Read_TOUR_SECTION(FILE ** File)
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