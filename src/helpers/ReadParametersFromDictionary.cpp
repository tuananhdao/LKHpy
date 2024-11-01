#include "ReadParametersFromDictionary.h"

/*
 * Alternative for LKH-3.0.12/SRC/ReadParameters.c
 * because reading from files is really slow
 */

void ReadParametersFromDictionary(py::dict params)
{
    char *Keyword, *Token, *Name;
    unsigned int i;

    ProblemFileName = PiFileName = InputTourFileName =
        OutputTourFileName = TourFileName = 0;
    CandidateFiles = MergeTourFiles = 0;
    AscentCandidates = 50;
    BackboneTrials = 0;
    Backtracking = 0;
    BWTSP_B = 0;
    BWTSP_Q = 0;
    BWTSP_L = INT_MAX;
    CandidateSetSymmetric = 0;
    CandidateSetType = ALPHA;
    Crossover = ERXT;
    DelaunayPartitioning = 0;
    DelaunayPure = 0;
    DemandDimension = 1;
    DistanceLimit = DBL_MAX;
    Excess = -1;
    ExternalSalesmen = 0;
    ExtraCandidates = 0;
    ExtraCandidateSetSymmetric = 0;
    ExtraCandidateSetType = QUADRANT;
    Gain23Used = 0;
    GainCriterionUsed = 1;
    GridSize = 1000000.0;
    InitialPeriod = -1;
    InitialStepSize = 0;
    InitialTourAlgorithm = WALK;
    InitialTourFraction = 1.0;
    KarpPartitioning = 0;
    KCenterPartitioning = 0;
    KMeansPartitioning = 0;
    Kicks = 1;
    KickType = 0;
    MaxBreadth = INT_MAX;
    MaxCandidates = 5;
    MaxPopulationSize = 0;
    MaxSwaps = -1;
    MaxTrials = -1;
    MoorePartitioning = 0;
    MoveType = 5;
    MoveTypeSpecial = 0;
    MTSPDepot = 1;
    MTSPMinSize = 1;
    MTSPMaxSize = -1;
    MTSPObjective = -1;
    NonsequentialMoveType = -1;
    Optimum = MINUS_INFINITY;
    PatchingA = 1;
    PatchingC = 0;
    PatchingAExtended = 0;
    PatchingARestricted = 0;
    PatchingCExtended = 0;
    PatchingCRestricted = 0;
    Precision = 100;
    Probability = 100;
    POPMUSIC_InitialTour = 0;
    POPMUSIC_MaxNeighbors = 5;
    POPMUSIC_SampleSize = 10;
    POPMUSIC_Solutions = 50;
    POPMUSIC_Trials = 1;
    Recombination = IPT;
    RestrictedSearch = 1;
    RohePartitioning = 0;
    Runs = 0;
    Salesmen = 1;
    Scale = -1;
    Seed = 1;
    SierpinskiPartitioning = 0;
    StopAtOptimum = 1;
    Subgradient = 1;
    SubproblemBorders = 0;
    SubproblemsCompressed = 0;
    SubproblemSize = 0;
    SubsequentMoveType = 0;
    SubsequentMoveTypeSpecial = 0;
    SubsequentPatching = 1;
    TimeLimit = DBL_MAX;
    TotalTimeLimit = DBL_MAX;
    TraceLevel = 1;
    TSPTW_Makespan = 0;

    ProblemFileName = "dummy.tsp";

    for (auto item : params) {
        std::string key = py::str(item.first);
        std::string value = py::str(item.second);
        std::string key_value = key + " = " + value;
        if (!(Keyword = strtok(&key_value[0], Delimiters)))
            continue;
        if (Keyword[0] == '#')
            continue;
        for (i = 0; i < strlen(Keyword); i++)
            Keyword[i] = (char) toupper(Keyword[i]);
        if (!strcmp(Keyword, "ASCENT_CANDIDATES")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &AscentCandidates))
                eprintf("ASCENT_CANDIDATES: integer expected");
            if (AscentCandidates < 2)
                eprintf("ASCENT_CANDIDATES: >= 2 expected");
        } else if (!strcmp(Keyword, "BACKBONE_TRIALS")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &BackboneTrials))
                eprintf("BACKBONE_TRIALS: integer expected");
            if (BackboneTrials < 0)
                eprintf("BACKBONE_TRIALS: non-negative integer expected");
        } else if (!strcmp(Keyword, "BACKTRACKING")) {
            if (!ReadYesOrNo(&Backtracking))
                eprintf("BACKTRACKING: YES or NO expected");
        } else if (!strcmp(Keyword, "BWTSP")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &BWTSP_B))
                eprintf("BWTSP: integer expected");
            if (BWTSP_B <= 0)
                eprintf("BWTSP: positive integer expected");
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &BWTSP_Q))
                eprintf("BWTSP: second integer expected");
            if (BWTSP_Q <= 0)
                eprintf("BWTSP: positive integer expected");
            if ((Token = strtok(0, Delimiters)) &&
                !sscanf(Token, "%d", &BWTSP_L))
                eprintf("BWTSP: third integer expected");
        } else if (!strcmp(Keyword, "CANDIDATE_FILE")) {
            if (!(Name = GetFileName(0)))
                eprintf("CANDIDATE_FILE: string expected");
            if (CandidateFiles == 0) {
                CandidateFileName = (char **) malloc(sizeof(char *));
                CandidateFileName[CandidateFiles++] = Name;
            } else {
                int i;
                for (i = 0; i < CandidateFiles; i++)
                    if (!strcmp(Name, CandidateFileName[i]))
                        break;
                if (i == CandidateFiles) {
                    CandidateFileName =
                       (char **) realloc(CandidateFileName,
                                         (CandidateFiles + 1) * sizeof(char *));
                    CandidateFileName[CandidateFiles++] = Name;
                }
            }
        } else if (!strcmp(Keyword, "CANDIDATE_SET_TYPE")) {
            if (!(Token = strtok(0, Delimiters)))
                eprintf("%s", "CANDIDATE_SET_TYPE: "
                        "ALPHA, DELAUNAY, NEAREST-NEIGHBOR, "
                        "POPMUSIC or QUADRANT expected");
            for (i = 0; i < strlen(Token); i++)
                Token[i] = (char) toupper(Token[i]);
            if (!strncmp(Token, "ALPHA", strlen(Token)))
                CandidateSetType = ALPHA;
            else if (!strncmp(Token, "DELAUNAY", strlen(Token)))
                CandidateSetType = DELAUNAY;
            else if (!strncmp(Token, "NEAREST-NEIGHBOR", strlen(Token)))
                CandidateSetType = NN;
            else if (!strncmp(Token, "POPMUSIC", strlen(Token)))
                CandidateSetType = POPMUSIC;
            else if (!strncmp(Token, "QUADRANT", strlen(Token)))
                CandidateSetType = QUADRANT;
            else
                eprintf("%s", "CANDIDATE_SET_TYPE: "
                        "ALPHA, DELAUNAY, NEAREST-NEIGHBOR, "
                        "POPMUSIC, or QUADRANT expected");
            if (CandidateSetType == DELAUNAY) {
                if ((Token = strtok(0, Delimiters))) {
                    for (i = 0; i < strlen(Token); i++)
                        Token[i] = (char) toupper(Token[i]);
                    if (strncmp(Token, "PURE", strlen(Token)))
                        eprintf("%s", "CANDIDATE_SET_TYPE (DELAUNAY): "
                                "PURE or no token expected");
                    DelaunayPure = 1;
                }
            }
        } else if (!strcmp(Keyword, "COMMENT")) {
            continue;
        } else if (!strcmp(Keyword, "DEPOT")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &MTSPDepot))
                eprintf("DEPOT: integer expected");
            if (MTSPDepot <= 0)
                eprintf("DEPOT: positive integer expected");
        } else if (!strcmp(Keyword, "DISTANCE")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%lf", &DistanceLimit))
                eprintf("DISTANCE: real expected");
            if (DistanceLimit < 0)
                eprintf("DISTANCE: >= 0 expected");
        } else if (!strcmp(Keyword, "EDGE_FILE")) {
            if (!(Name = GetFileName(0)))
                eprintf("EDGE_FILE: string expected");
            if (EdgeFiles == 0) {
                EdgeFileName = (char **) malloc(sizeof(char *));
                EdgeFileName[EdgeFiles++] = Name;
            } else {
                int i;
                for (i = 0; i < EdgeFiles; i++)
                    if (!strcmp(Name, EdgeFileName[i]))
                        break;
                if (i == EdgeFiles) {
                    EdgeFileName =
                       (char **) realloc(EdgeFileName,
                                         (EdgeFiles + 1) * sizeof(char *));
                    EdgeFileName[EdgeFiles++] = Name;
                }
            }
        } else if (!strcmp(Keyword, "EOF")) {
            break;
        } else if (!strcmp(Keyword, "EXCESS")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%lf", &Excess))
                eprintf("EXCESS: real expected");
            if (Excess < 0)
                eprintf("EXCESS: non-negeative real expected");
        } else if (!strcmp(Keyword, "EXTERNAL_SALESMEN")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &ExternalSalesmen))
                eprintf("%s: integer expected", Keyword);
            if (ExternalSalesmen < 0)
                eprintf("%s: non-negative integer expected", Keyword);
        } else if (!strcmp(Keyword, "EXTRA_CANDIDATES")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &ExtraCandidates))
                eprintf("EXTRA_CANDIDATES: integer expected");
            if (ExtraCandidates < 0)
                eprintf("EXTRA_CANDIDATES: non-negative integer expected");
            if ((Token = strtok(0, Delimiters))) {
                for (i = 0; i < strlen(Token); i++)
                    Token[i] = (char) toupper(Token[i]);
                if (strncmp(Token, "SYMMETRIC", strlen(Token)))
                    eprintf
                        ("(EXTRA_CANDIDATES) Illegal SYMMETRIC specification");
                ExtraCandidateSetSymmetric = 1;
            }
        } else if (!strcmp(Keyword, "EXTRA_CANDIDATE_SET_TYPE")) {
            if (!(Token = strtok(0, Delimiters)))
                eprintf("%s", "EXTRA_CANDIDATE_SET_TYPE: "
                        "NEAREST-NEIGHBOR, POPMUSIC or QUADRANT expected");
            for (i = 0; i < strlen(Token); i++)
                Token[i] = (char) toupper(Token[i]);
            if (!strncmp(Token, "NEAREST-NEIGHBOR", strlen(Token)))
                ExtraCandidateSetType = NN;
            else if (!strncmp(Token, "POPMUSIC", strlen(Token)))
                ExtraCandidateSetType = POPMUSIC;
            else if (!strncmp(Token, "QUADRANT", strlen(Token)))
                ExtraCandidateSetType = QUADRANT;
            else
                eprintf("%s", "EXTRA_CANDIDATE_SET_TYPE: "
                        "NEAREST-NEIGHBOR, POPMUSIC or QUADRANT expected");
        } else if (!strcmp(Keyword, "GAIN23")) {
            if (!ReadYesOrNo(&Gain23Used))
                eprintf("GAIN23: YES or NO expected");
        } else if (!strcmp(Keyword, "GAIN_CRITERION")) {
            if (!ReadYesOrNo(&GainCriterionUsed))
                eprintf("GAIN_CRITERION: YES or NO expected");
        } else if (!strcmp(Keyword, "INITIAL_PERIOD")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &InitialPeriod))
                eprintf("INITIAL_PERIOD: integer expected");
            if (InitialPeriod < 0)
                eprintf("INITIAL_PERIOD: non-negative integer expected");
        } else if (!strcmp(Keyword, "INITIAL_STEP_SIZE")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &InitialStepSize))
                eprintf("INITIAL_STEP_SIZE: integer expected");
            if (InitialStepSize <= 0)
                eprintf("INITIAL_STEP_SIZE: positive integer expected");
        } else if (!strcmp(Keyword, "INITIAL_TOUR_ALGORITHM")) {
            if (!(Token = strtok(0, Delimiters)))
                eprintf("INITIAL_TOUR_ALGORITHM: "
                        "BORUVKA, CTSP, CVRP, GREEDY, MOORE, MTSP,\n"
                        "NEAREST-NEIGHBOR, QUICK-BORUVKA, SIERPINSKI, "
                        "SOP, TSPDL, or WALK expected");
            for (i = 0; i < strlen(Token); i++)
                Token[i] = (char) toupper(Token[i]);
            if (!strncmp(Token, "BORUVKA", strlen(Token)))
                InitialTourAlgorithm = BORUVKA;
            else if (!strncmp(Token, "CTSP", strlen(Token)))
                InitialTourAlgorithm = CTSP_ALG;
            else if (!strncmp(Token, "GCTSP", strlen(Token)))
                InitialTourAlgorithm = GCTSP_ALG;
            else if (!strncmp(Token, "CVRP", strlen(Token)))
                InitialTourAlgorithm = CVRP_ALG;
            else if (!strncmp(Token, "GREEDY", strlen(Token)))
                InitialTourAlgorithm = GREEDY;
            else if (!strncmp(Token, "MOORE", max(strlen(Token), 2)))
                InitialTourAlgorithm = MOORE;
            else if (!strncmp(Token, "MTSP", max(strlen(Token), 2)))
                InitialTourAlgorithm = MTSP_ALG;
            else if (!strncmp(Token, "NEAREST-NEIGHBOR", strlen(Token)))
                InitialTourAlgorithm = NEAREST_NEIGHBOR;
            else if (!strncmp(Token, "QUICK-BORUVKA", strlen(Token)))
                InitialTourAlgorithm = QUICK_BORUVKA;
            else if (!strncmp(Token, "SIERPINSKI", max(strlen(Token), 2)))
                InitialTourAlgorithm = SIERPINSKI;
            else if (!strncmp(Token, "SOP", max(strlen(Token), 2)))
                InitialTourAlgorithm = SOP_ALG;
            else if (!strncmp(Token, "TSPDL", strlen(Token)))
                InitialTourAlgorithm = TSPDL_ALG;
            else if (!strncmp(Token, "WALK", strlen(Token)))
                InitialTourAlgorithm = WALK;
            else
                eprintf("INITIAL_TOUR_ALGORITHM: "
                        "BORUVKA, CVRP, GREEDY, MOORE, MTSP,\n"
                        "NEAREST-NEIGHBOR, QUICK-BORUVKA, SIERPINSKI, "
                        "SOP, TSPDL, or WALK expected");
        } else if (!strcmp(Keyword, "INITIAL_TOUR_FILE")) {
            if (!(InitialTourFileName = GetFileName(0)))
                eprintf("INITIAL_TOUR_FILE: string expected");
        } else if (!strcmp(Keyword, "INITIAL_TOUR_FRACTION")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%lf", &InitialTourFraction))
                eprintf("INITIAL_TOUR_FRACTION: real expected");
            if (InitialTourFraction < 0 || InitialTourFraction > 1)
                eprintf("INITIAL_TOUR_FRACTION: >= 0 or <= 1 expected");
        } else if (!strcmp(Keyword, "INPUT_TOUR_FILE")) {
            if (!(InputTourFileName = GetFileName(0)))
                eprintf("INPUT_TOUR_FILE: string expected");
        } else if (!strcmp(Keyword, "KICK_TYPE")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &KickType))
                eprintf("KICK_TYPE: integer expected");
            if (KickType != 0 && KickType < 4)
                eprintf("KICK_TYPE: integer >= 4 expected");
        } else if (!strcmp(Keyword, "K")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &k))
                eprintf("k: integer expected");
            if (k <= 0)
                eprintf("K: positive integer expected");
        } else if (!strcmp(Keyword, "KICKS")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &Kicks))
                eprintf("KICKS: integer expected");
            if (Kicks < 0)
                eprintf("KICKS: non-negative integer expected");
        } else if (!strcmp(Keyword, "MAKESPAN")) {
            if (!ReadYesOrNo(&TSPTW_Makespan))
                eprintf("MAKESPAN: YES or NO expected");
        } else if (!strcmp(Keyword, "MAX_BREADTH")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &MaxBreadth))
                eprintf("MAX_BREADTH: integer expected");
            if (MaxBreadth < 0)
                eprintf("MAX_BREADTH: non-negative integer expected");
        } else if (!strcmp(Keyword, "MAX_CANDIDATES")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &MaxCandidates))
                eprintf("MAX_CANDIDATES: integer expected");
            if (MaxCandidates < 0)
                eprintf("MAX_CANDIDATES: non-negative integer expected");
            if ((Token = strtok(0, Delimiters))) {
                for (i = 0; i < strlen(Token); i++)
                    Token[i] = (char) toupper(Token[i]);
                if (!strncmp(Token, "SYMMETRIC", strlen(Token)))
                    CandidateSetSymmetric = 1;
                else
                    eprintf
                        ("(MAX_CANDIDATES) Illegal SYMMETRIC specification");
            }
        } else if (!strcmp(Keyword, "MAX_SWAPS")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &MaxSwaps))
                eprintf("MAX_SWAPS: integer expected");
            if (MaxSwaps < 0)
                eprintf("MAX_SWAPS: non-negative integer expected");
        } else if (!strcmp(Keyword, "MAX_TRIALS")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &MaxTrials))
                eprintf("MAX_TRIALS: integer expected");
            if (MaxTrials < 0)
                eprintf("MAX_TRIALS: non-negative integer expected");
        } else if (!strcmp(Keyword, "MERGE_TOUR_FILE")) {
            if (!(Name = GetFileName(0)))
                eprintf("MERGE_TOUR_FILE: string expected");
            if (MergeTourFiles == 0) {
                MergeTourFileName = (char **) malloc(sizeof(char *));
                MergeTourFileName[MergeTourFiles++] = Name;
            } else {
                int i;
                for (i = 0; i < MergeTourFiles; i++)
                    if (!strcmp(Name, MergeTourFileName[i]))
                        break;
                if (i == MergeTourFiles) {
                    MergeTourFileName =
                       (char **) realloc(MergeTourFileName,
                                         (MergeTourFiles + 1) * sizeof(char *));
                    MergeTourFileName[MergeTourFiles++] = Name;
                }
            }
        } else if (!strcmp(Keyword, "MOVE_TYPE")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &MoveType))
                eprintf("MOVE_TYPE: integer expected");
            if (MoveType < 2)
                eprintf("MOVE_TYPE: >= 2 expected");
            MoveTypeSpecial = 0;
            if ((Token = strtok(0, Delimiters))) {
                for (i = 0; i < strlen(Token); i++)
                    Token[i] = (char) toupper(Token[i]);
                if (!strncmp(Token, "SPECIAL", strlen(Token)))
                    MoveTypeSpecial = 1;
                else
                    eprintf("%s", "(MOVE_TYPE) "
                            "Illegal SPECIAL specification");
                if (MoveType != 3 && MoveType != 5)
                    eprintf("%s", "(MOVE_TYPE) "
                            "SPECIAL, MOVE_TYPE must be 3 or 5");
            }
        } else if (!strcmp(Keyword, "MTSP_MAX_SIZE")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &MTSPMaxSize))
                eprintf("MTSP_MAX_SIZE: integer expected");
            if (MTSPMaxSize <= 0)
                eprintf("MTSP_MAX_SIZE: positive integer expected");
        } else if (!strcmp(Keyword, "MTSP_MIN_SIZE")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &MTSPMinSize))
                eprintf("MTSP_MIN_SIZE: integer expected");
        } else if (!strcmp(Keyword, "MTSP_OBJECTIVE")) {
            if ((Token = strtok(0, Delimiters))) {
                for (i = 0; i < strlen(Token); i++)
                    Token[i] = (char) toupper(Token[i]);
                if (!strcmp(Token, "MINMAX"))
                    MTSPObjective = MINMAX;
                else if (!strcmp(Token, "MINMAX_SIZE"))
                    MTSPObjective = MINMAX_SIZE;
                else if (!strcmp(Token, "MINSUM"))
                    MTSPObjective = MINSUM;
                else
                    eprintf("MTSP_OBJECTIVE: MINMAX, MINMAX_SIZE, or "
                            "MINSUM expected");
            } else
                eprintf("MTSP_OBJECTIVE: MINMAX, MINMAX_SIZE, or "
                        "MINSUM expected");
        } else if (!strcmp(Keyword, "MTSP_SOLUTION_FILE")) {
            if (!(MTSPSolutionFileName = GetFileName(0)))
                eprintf("MTSP_SOLUTION_FILE: string expected");
        } else if (!strcmp(Keyword, "NONSEQUENTIAL_MOVE_TYPE")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &NonsequentialMoveType))
                eprintf("NONSEQUENTIAL_MOVE_TYPE: integer expected");
            if (NonsequentialMoveType < 4)
                eprintf("NONSEQUENTIAL_MOVE_TYPE: >= 4 expected");
        } else if (!strcmp(Keyword, "OPTIMUM")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, GainInputFormat, &Optimum))
                eprintf("OPTIMUM: integer expected");
        } else if (!strcmp(Keyword, "OUTPUT_TOUR_FILE")) {
            if (!(OutputTourFileName = GetFileName(0)))
                eprintf("OUTPUT_TOUR_FILE: string expected");
        } else if (!strcmp(Keyword, "PATCHING_A")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &PatchingA))
                eprintf("PATCHING_A: integer expected");
            if (PatchingA < 0)
                eprintf("PATCHING_A: non-negative integer expected");
            if ((Token = strtok(0, Delimiters))) {
                for (i = 0; i < strlen(Token); i++)
                    Token[i] = (char) toupper(Token[i]);
                if (!strncmp(Token, "RESTRICTED", strlen(Token)))
                    PatchingARestricted = 1;
                else if (!strncmp(Token, "EXTENDED", strlen(Token)))
                    PatchingAExtended = 1;
                else
                    eprintf("%s", "(PATCHING_A) "
                            "Illegal RESTRICTED or EXTENDED specification");
            }
        } else if (!strcmp(Keyword, "PATCHING_C")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &PatchingC))
                eprintf("PATCHING_C: integer expected");
            if (PatchingC < 0)
                eprintf("PATCHING_C: non-negative integer expected");
            if ((Token = strtok(0, Delimiters))) {
                for (i = 0; i < strlen(Token); i++)
                    Token[i] = (char) toupper(Token[i]);
                if (!strncmp(Token, "RESTRICTED", strlen(Token)))
                    PatchingCRestricted = 1;
                else if (!strncmp(Token, "EXTENDED", strlen(Token)))
                    PatchingCExtended = 1;
                else
                    eprintf("%s", "(PATCHING_C) ",
                            "Illegal RESTRICTED or EXTENDED specification");
            }
        } else if (!strcmp(Keyword, "PI_FILE")) {
            if (!(PiFileName = GetFileName(0)))
                eprintf("PI_FILE: string expected");
        } else if (!strcmp(Keyword, "POPMUSIC_INITIAL_TOUR")) {
            if (!ReadYesOrNo(&POPMUSIC_InitialTour))
                eprintf("POPMUSIC_INITIAL_TOUR: YES or NO expected");
        } else if (!strcmp(Keyword, "POPMUSIC_MAX_NEIGHBORS")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &POPMUSIC_MaxNeighbors))
                eprintf("POPMUSIC_MAX_NEIGHBORS: integer expected");
            if (POPMUSIC_MaxNeighbors <= 0)
                eprintf
                    ("POPMUSIC_MAX_NEIGHBORS: positive integer expected");
        } else if (!strcmp(Keyword, "POPMUSIC_SAMPLE_SIZE")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &POPMUSIC_SampleSize))
                eprintf("POPMUSIC_SAMPLE_SIZE: integer expected");
            if (POPMUSIC_SampleSize <= 0)
                eprintf("POPMUSIC_SAMPLE_SIZE: positive integer expected");
        } else if (!strcmp(Keyword, "POPMUSIC_SOLUTIONS")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &POPMUSIC_Solutions))
                eprintf("POPMUSIC_SOLUTIONS: integer expected");
            if (POPMUSIC_Solutions <= 0)
                eprintf("POPMUSIC_SOLUTIONS: positive integer expected");
        } else if (!strcmp(Keyword, "POPMUSIC_TRIALS")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &POPMUSIC_Trials))
                eprintf("POPMUSIC_TRIALS: integer expected");
            if (POPMUSIC_Trials < 0)
                eprintf("POPMUSIC_TRIALS: non-negative integer expected");
        } else if (!strcmp(Keyword, "POPULATION_SIZE")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &MaxPopulationSize))
                eprintf("POPULATION_SIZE: integer expected");
        } else if (!strcmp(Keyword, "PRECISION")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &Precision))
                eprintf("PRECISION: integer expected");
        } else if (!strcmp(Keyword, "PROBABILITY")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &Probability))
                eprintf("PROBABILITY: integer expected");
            if (Probability < 0)       
                eprintf("PROBABILITY: >= 0 expected");
            if (Probability > 100)       
                eprintf("PROBABILITY: <= 100 expected");
        } else if (!strcmp(Keyword, "PROBLEM_FILE")) {
            if (!(ProblemFileName = GetFileName(0)))
                eprintf("PROBLEM_FILE: string expected");
        } else if (!strcmp(Keyword, "RECOMBINATION")) {
             if (!(Token = strtok(0, Delimiters)))
                eprintf("RECOMBINATION: string expected");
            if (!strncmp(Token, "IPT", strlen(Token)))
                Recombination = IPT;
            else if (!strncmp(Token, "GPX2", strlen(Token)))
                Recombination = GPX2;
            else if (!strncmp(Token, "CLARIST", strlen(Token)))
                Recombination = CLARIST;
            else
                eprintf("RECOMBINATION: IPT, GPX2 or CLARIST expected");
        } else if (!strcmp(Keyword, "RESTRICTED_SEARCH")) {
            if (!ReadYesOrNo(&RestrictedSearch))
                eprintf("RESTRICTED_SEARCH: YES or NO expected");
        } else if (!strcmp(Keyword, "RUNS")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &Runs))
                eprintf("RUNS: integer expected");
            if (Runs <= 0)
                eprintf("RUNS: positive integer expected");
        } else if (!strcmp(Keyword, "SALESMEN") ||
                   !strcmp(Keyword, "VEHICLES")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &Salesmen))
                eprintf("%s: integer expected", Keyword);
            if (Salesmen <= 0)
                eprintf("%s: positive integer expected", Keyword);
        } else if (!strcmp(Keyword, "SCALE")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &Scale))
                eprintf("SCALE: integer expected");
        } else if (!strcmp(Keyword, "SEED")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%u", &Seed))
                eprintf("SEED: integer expected");
        } else if (!strcmp(Keyword, "SPECIAL")) {
            Gain23Used = 0;
            KickType = 4;
            MaxSwaps = 0;
            MoveType = 5;
            MoveTypeSpecial = 1;
            MaxPopulationSize = 10;
        } else if (!strcmp(Keyword, "SINTEF_SOLUTION_FILE")) {
            if (!(SINTEFSolutionFileName = GetFileName(0)))
                eprintf("SINTEF_SOLUTION_FILE: string expected");
        } else if (!strcmp(Keyword, "STOP_AT_OPTIMUM")) {
            if (!ReadYesOrNo(&StopAtOptimum))
                eprintf("STOP_AT_OPTIMUM: YES or NO expected");
        } else if (!strcmp(Keyword, "SUBGRADIENT")) {
            if (!ReadYesOrNo(&Subgradient))
                eprintf("SUBGRADIENT: YES or NO expected");
        } else if (!strcmp(Keyword, "SUBPROBLEM_TOUR_FILE")) {
            if (!(SubproblemTourFileName = GetFileName(0)))
                eprintf("SUBPROBLEM_TOUR_FILE: string expected");
        } else if (!strcmp(Keyword, "SUBPROBLEM_SIZE")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &SubproblemSize))
                eprintf("SUBPROBLEM_SIZE: integer expected");
            if (SubproblemSize < 3)
                eprintf("SUBPROBLEM_SIZE: >= 3 expected");
            if ((Token = strtok(0, Delimiters))) {
                for (i = 0; i < strlen(Token); i++)
                    Token[i] = (char) toupper(Token[i]);
                if (!strncmp(Token, "DELAUNAY", strlen(Token)))
                    DelaunayPartitioning = 1;
                else if (!strncmp(Token, "KARP", max(strlen(Token), 2)))
                    KarpPartitioning = 1;
                else if (!strncmp
                         (Token, "K-CENTER", max(strlen(Token), 3)))
                    KCenterPartitioning = 1;
                else if (!strncmp(Token, "K-MEANS", max(strlen(Token), 3)))
                    KMeansPartitioning = 1;
                else if (!strncmp(Token, "MOORE", strlen(Token)))
                    MoorePartitioning = 1;
                else if (!strncmp(Token, "ROHE", strlen(Token)))
                    RohePartitioning = 1;
                else if (!strncmp(Token, "SIERPINSKI", strlen(Token)))
                    SierpinskiPartitioning = 1;
                else if (!strncmp(Token, "BORDERS", strlen(Token)))
                    SubproblemBorders = 1;
                else if (!strncmp(Token, "COMPRESSED", strlen(Token)))
                    SubproblemsCompressed = 1;
                else
                    eprintf
                        ("(SUBPROBLEM_SIZE) Illegal DELAUNAY, KARP, K-CENTER, "
                         "K-MEANS, MOORE, ROHE,\n SIERPINSKI, "
                         "BORDERS or COMPRESSED specification");
                while ((Token = strtok(0, Delimiters))) {
                    for (i = 0; i < strlen(Token); i++)
                        Token[i] = (char) toupper(Token[i]);
                    if (!strncmp(Token, "BORDERS", strlen(Token)))
                        SubproblemBorders = 1;
                    else if (!strncmp(Token, "COMPRESSED", strlen(Token)))
                        SubproblemsCompressed = 1;
                    else
                        eprintf
                            ("(SUBPROBLEM_SIZE) Illegal BORDERS or "
                             "COMPRESSED specification");
                }
            }
        } else if (!strcmp(Keyword, "SUBSEQUENT_MOVE_TYPE")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &SubsequentMoveType))
                eprintf("SUBSEQUENT_MOVE_TYPE: integer expected");
            if (SubsequentMoveType != 0 && SubsequentMoveType < 2)
                eprintf("SUBSEQUENT_MOVE_TYPE: 0 or >= 2 expected");
            SubsequentMoveTypeSpecial = 0;
            if ((Token = strtok(0, Delimiters))) {
                for (i = 0; i < strlen(Token); i++)
                    Token[i] = (char) toupper(Token[i]);
                if (!strncmp(Token, "SPECIAL", strlen(Token)))
                    SubsequentMoveTypeSpecial = 1;
                else
                    eprintf("%s", "(SUBSEQUENT_MOVE_TYPE) "
                            "Illegal SPECIAL specification");
                if (SubsequentMoveType != 3 && SubsequentMoveType != 5)
                    eprintf("%s", "(SUBSEQUENT_MOVE_TYPE) "
                            "SPECIAL, SUBSEQUENT_MOVE_TYPE must be 3 or 5");
            }
        } else if (!strcmp(Keyword, "SUBSEQUENT_PATCHING")) {
            if (!ReadYesOrNo(&SubsequentPatching))
                eprintf("SUBSEQUENT_PATCHING: YES or NO expected");
        } else if (!strcmp(Keyword, "TIME_LIMIT")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%lf", &TimeLimit))
                eprintf("TIME_LIMIT: real expected");
            if (TimeLimit < 0)
                eprintf("TIME_LIMIT: >= 0 expected");
        } else if (!strcmp(Keyword, "TOTAL_TIME_LIMIT")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%lf", &TotalTimeLimit))
                eprintf("TOTAL_TIME_LIMIT: real expected");
            if (TimeLimit < 0)
                eprintf("TOTAL_TIME_LIMIT: >= 0 expected");
        } else if (!strcmp(Keyword, "TOUR_FILE")) {
            if (!(TourFileName = GetFileName(0)))
                eprintf("TOUR_FILE: string expected");
        } else if (!strcmp(Keyword, "TRACE_LEVEL")) {
            if (!(Token = strtok(0, Delimiters)) ||
                !sscanf(Token, "%d", &TraceLevel))
                eprintf("TRACE_LEVEL: integer expected");
        } else
            eprintf("Unknown keyword: %s", Keyword);
        if ((Token = strtok(0, Delimiters)) && Token[0] != '#')
            eprintf("Junk at end of line: %s", Token);
    }
    if (!ProblemFileName)
        eprintf("Problem file name is missing");
    if (SubproblemSize == 0 && SubproblemTourFileName != 0)
        eprintf("SUBPROBLEM_SIZE specification is missing");
    if (SubproblemSize > 0 && SubproblemTourFileName == 0)
        eprintf("SUBPROBLEM_TOUR_FILE specification is missing");
    if (SubproblemSize > 0 && Salesmen > 1)
        eprintf("SUBPROBLEM specification not possible for SALESMEN > 1");
    if (CandidateSetType != DELAUNAY)
        DelaunayPure = 0;
    free(LastLine);
    LastLine = 0;
}

static char *GetFileName(char *Line)
{
    char *Rest = strtok(Line, "\n\t\r\f"), *t;

    if (!Rest)
        return 0;
    while (isspace(*Rest))
        Rest++;
    if (!Line) {
        if (*Rest == '=')
            Rest++;
    }
    while (isspace(*Rest))
        Rest++;
    for (t = Rest + strlen(Rest) - 1; isspace(*t); t--)
        *t = '\0';
    if (!strlen(Rest))
        return 0;
    t = (char *) malloc(strlen(Rest) + 1);
    strcpy(t, Rest);
    return t;
}

static char *ReadYesOrNo(int *V)
{
    char *Token = strtok(0, Delimiters);

    if (Token) {
        unsigned int i;
        for (i = 0; i < strlen(Token); i++)
            Token[i] = (char) toupper(Token[i]);
        if (!strncmp(Token, "YES", strlen(Token)))
            *V = 1;
        else if (!strncmp(Token, "NO", strlen(Token)))
            *V = 0;
        else
            Token = 0;
    }
    return Token;
}

static size_t max(size_t a, size_t b)
{
    return a > b ? a : b;
}