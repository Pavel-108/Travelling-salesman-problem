#include <stdio.h>

///PROGRAM TSP_PMX
///=========================
///Program vyhodnocuje delku optimalni cesty a jeji prubeh dle specifikace problemu obchodniho cestujiciho,
///vyuziva genetickych algoritmu, kde pro urcovani potomku pouziva algoritmus PMX (partially matched crossover),
///zadanim programu je pocet vrcholu, matice sousednosti a pocet generaci,
///program v prubehu vypisuje aktualni nejlepsi cestu, cislo generace v ktere byla vyhodnocena a delky cest jednotlivych permutaci
///(zde je mozne pozorovat vyvoj populace a jeji evolucni zlepsovani),
///po uplynuti poctu generaci program konci a vypisuje finalni vysledek.

///STRUKTURA PROGRAMU
///=========================
/// 0. Nacti ze souboru "test_number_of_vertex" a "test_number_of_generations"
/// 1. Nacti ze souboru "test_adjacency_matrix"
/// 2.1. Vytvor "array_of_number_of_permutations"
///         - pole obsahujici cisla permutaci obsazene v populaci
/// 2.2. Vytvor "test_array_of_permutations"
///         - pole obsahujici permutace s cisly permutaci (urcene poradim) z "array_of_number_of_permutations"
/// 3. vyhodnot value_of_permutation
///         - zapamatuj si 2 nejlepsi a 2 nejhorsi
/// 4. Cross function - PMX (partially matched crossover)
///         - ze 2 nejlepsich ("parents") vytvor potomky ("offsprings"), potomky nahrad 2 nejhorsi ("loosers"), pokud jsou horsi nez "offsprings"
/// 5. pokracuj krokem 3. az do urceneho poctu generaci
/// 6. vypis vysledek

///PROGRAM
///=========================
const int MAX_INT = sizeof(int);

int test_number_of_vertex;
int test_number_of_start_permutations;

void loadFileBase(const char *filename)
{
    FILE *file = fopen(filename, "r");
    fscanf(file, "%d %d", &test_number_of_vertex, &test_number_of_start_permutations);
}


///CLASS TSP
class TravellingSalesmanProblem
{
public:
    ///DECLARATION OF VARIABLES
    int test_number_of_generations;
    int number_of_actual_permutation;
    int cross_point;
    int find_for;

    int lenght_of_parent_1_Min;
    int lenght_of_parent_2_Min2;
    int lenght_of_looser_1_Max;
    int lenght_of_looser_2_Max2;
    int length_of_offspring_1;
    int length_of_offspring_2;
    int number_of_looser_1;
    int number_of_looser_2;

    int * parent_1;
    int * parent_2;
    int * looser_1;
    int * looser_2;
    int * offspring_1;
    int * offspring_2;
    int * array_of_number_of_permutations; //Obsahuje cisla permutaci, ktere budou vzbrany jako zaklad
    int * array_of_actual_per;
    int * array_of_transform;
    int **test_array_of_permutations;
    int **test_adjacency_matrix;


    ///KONSTRUKTOR + DESTRUKTOR
    TravellingSalesmanProblem(int number_of_vertex, int number_of_start_permutations)
    {
        number_of_actual_permutation = 0;

        lenght_of_parent_1_Min = 100000;
        lenght_of_parent_2_Min2 = 100000;
        lenght_of_looser_1_Max = 0;
        lenght_of_looser_2_Max2 = 0;

        parent_1 = new int [test_number_of_vertex];
        parent_2 = new int [test_number_of_vertex];
        looser_1 = new int [test_number_of_vertex];
        looser_2 = new int [test_number_of_vertex];
        offspring_1 = new int [test_number_of_vertex];
        offspring_2 = new int [test_number_of_vertex];
        array_of_transform = new int [test_number_of_vertex];
        array_of_number_of_permutations = new int [test_number_of_start_permutations];
        array_of_actual_per = new int [test_number_of_vertex];

        test_array_of_permutations = new int* [test_number_of_start_permutations];
        for (int i = 0; i < test_number_of_start_permutations; i++)
        {
            test_array_of_permutations[i] = new int [test_number_of_vertex];
        }
    }
    ~TravellingSalesmanProblem()
    {
        for (int i = 0; i < test_number_of_vertex; i++)
        {
            delete test_adjacency_matrix[i];
        }
        delete[] test_adjacency_matrix;

        for (int i = 0; i < test_number_of_vertex; i++)
        {
            delete test_array_of_permutations[i];
        }
        delete[] test_array_of_permutations;

        delete[] parent_1;
        delete[] parent_2;
        delete[] looser_1;
        delete[] looser_2;
        delete[] offspring_1;
        delete[] offspring_2;
        delete[] array_of_transform;
    }

    void loadFile(const char *filename)
    {
        FILE *file = fopen(filename, "r");
        fscanf(file, "%d %d %d", &test_number_of_vertex, &test_number_of_start_permutations, &test_number_of_generations);

        test_adjacency_matrix = new int* [test_number_of_vertex];
        for (int i = 0; i < test_number_of_vertex; i++)
        {
            test_adjacency_matrix[i] = new int [test_number_of_vertex];
        }

        for (int i = 0; i < test_number_of_vertex; i++)
        {
            for (int j = 0; j < test_number_of_vertex; j++)
            {
                fscanf(file, "%d", &test_adjacency_matrix[i][j]);
            }
        }
    }


    /// ------------------------------------------BEGIN-TVORBA-POLE-PERMUTACI------------------------------------------------------------
    ///TVORBA POLE PERMUTACI
    void createArrayOfNumberOfPermutations(int number_of_vertex, int number_of_start_permutations)
    {
        for (int j = 0; j < (test_number_of_start_permutations); j++)
        {
            array_of_number_of_permutations[j] = j;
        }
    }

    void isThisStartPermutation()
    {
        if (number_of_actual_permutation == array_of_number_of_permutations[number_of_actual_permutation])
        {
            for (int j = 0; j < test_number_of_vertex; j++)
            {
                test_array_of_permutations[number_of_actual_permutation][j] = array_of_actual_per[j];
            }
            number_of_actual_permutation++;
        }
    }

    void printArrayOfStartPermutations(int number_of_gen, int number_of_ver)
    {
        for (int i = 0; i < number_of_gen; i++)
        {
            for (int j = 0; j < number_of_ver; j++)
            {
                printf("%d " , test_array_of_permutations[i][j]);
            }
            printf("\n");
        }
    }

    void printPermutation(int size)
    {
        for (int j = 0; j < size; j++)
        {
            printf("%d " , array_of_actual_per[j]);
        }
    }

    void swap(int x, int y)
    {
        int temp = array_of_actual_per[x];
        array_of_actual_per[x] = array_of_actual_per[y];
        array_of_actual_per[y] = temp;
    }

    void permute(int k, int size)
    {
       int i;

       if (k==0)
       {
           isThisStartPermutation();
       }
       else
       {
           for (int i=k-1; i>=0; i--)
           {
               swap(i, k-1);
               permute(k-1, size);
               swap(i, k-1);
           }
       }
    }

    void createArrayOfPermutations()
    {
        for (int j = 0; j < test_number_of_vertex; j++)
        {
            array_of_actual_per[j] = j;
        }

        permute(test_number_of_vertex, test_number_of_vertex);
    }

    void printArrayOfNumberOfPermutations(int size)
    {
        for (int j = 0; j < size; j++)
        {
            printf("%d " , array_of_number_of_permutations[j]);
        }
    }
    /// ------------------------------------------END----------------------------------------------------------------


    /// ------------------------------------------BEGIN-HLEDEJ-PARENTS-A-LOOSERS-----------------------------------------------------------
    ///Vyhodnot rank permutaci, najdi 2 nejlepsi (parent_1, parent_2) a 2 nejhorsi (looser_1, looser_2;
    ///budeme nahrazovat, staci cisla permutaci - number_of_looser_1, number_of_looser_2)
    ///dale se budou porovnavat s potomky, tedy si pamatujeme i jejich hodnoty lenght_of_looser_1_Max, lenght_of_looser_2_Max2
    ///Pracuji pouze s polem aktualnich permutaci - test_array_of_permutations, ktere se aktualizuje
    void print1DArray(int *arr, int size, int number_of_parent)
    {
        printf("parent %d:\n", number_of_parent);

        for (int j = 0; j < size; j++)
        {
            printf("%d " , arr[j]);
        }

        printf("\n");
    }

    void transform1DArrayToActual(int *arr_transform, int size, int number_of_act)
    {
        for (int i = 0; i<size; i++)
        {
            arr_transform[i] = test_array_of_permutations[number_of_act][i];
        }
    }

    ///z A se stane B
    void transformArrayAtoArrayB(int *arr_transform_A, int *arr_transform_B, int size)
    {
        for (int i = 0; i<size; i++)
        {
            arr_transform_A[i] = arr_transform_B[i];
        }
    }

    void printMinMax(int size)
    {
        printf("min1: %d\n", lenght_of_parent_1_Min);
        printf("min2: %d\n", lenght_of_parent_2_Min2);
        printf("max1: %d and number of this perm. is: %d\n", lenght_of_looser_1_Max, number_of_looser_1);
        printf("max2: %d and number of this perm. is: %d\n", lenght_of_looser_2_Max2, number_of_looser_2);
        print1DArray(parent_1, test_number_of_vertex, 1);
        print1DArray(parent_2, test_number_of_vertex, 2);
    }

    void compareActualWithMinMax(int length_of_actual, int number_of_actual)
    {
        printf("length of %d permutaion is: %d\n", number_of_actual, length_of_actual);

        if (length_of_actual < lenght_of_parent_1_Min)
        {
            lenght_of_parent_2_Min2 = lenght_of_parent_1_Min;
            lenght_of_parent_1_Min = length_of_actual;

            transformArrayAtoArrayB(parent_2, parent_1, test_number_of_vertex);
            transform1DArrayToActual(parent_1, test_number_of_vertex, number_of_actual);
        }
        else
        {
            if (length_of_actual < lenght_of_parent_2_Min2)
            {
                lenght_of_parent_2_Min2 = length_of_actual;
                transform1DArrayToActual(parent_2, test_number_of_vertex, number_of_actual);
            }
        }

        if (length_of_actual > lenght_of_looser_1_Max)
        {
            lenght_of_looser_2_Max2 = lenght_of_looser_1_Max;
            lenght_of_looser_1_Max = length_of_actual;
            number_of_looser_2 = number_of_looser_1;
            number_of_looser_1 = number_of_actual;
        }
        else
        {
            if (length_of_actual > lenght_of_looser_2_Max2)
            {

            }
                lenght_of_looser_2_Max2 = length_of_actual;
                number_of_looser_2 = number_of_actual;
        }
    }

    void lengthOfActualPermutation(int size, int number_of_actual_perm)
    {
       int length_of_actual_per = 0;

       for (int i=0; i<(size-1); i++)
       {
           length_of_actual_per = length_of_actual_per + test_adjacency_matrix[test_array_of_permutations[number_of_actual_perm][i]][test_array_of_permutations[number_of_actual_perm][i+1]];
       }

       compareActualWithMinMax(length_of_actual_per, number_of_actual_perm);
    }

    void findParentsAndLoosers(int number_of_start_per)
    {
        for (int i=0; i<(number_of_start_per); i++)
        {
            lengthOfActualPermutation(test_number_of_vertex, i);
        }
    }
    /// ------------------------------------------END----------------------------------------------------------------


    /// ------------------------------------------BEGIN-CROSS--------------------------------------------------------------
    ///CROSS parent_1 a parent_2
    ///vytvori offsprings, zkontroluje zda jsou lepsi nez looser_1 a looser_2, pokud ano nahradi
    int lengthOfOffspring(int *arr, int size)
    {
       int length = 0;

       for (int i=0; i<(size-1); i++)
       {
           length = length + test_adjacency_matrix[arr[i]][arr[i+1]];
       }

       return length;
    }

    void transformLooserToOffspring(int *arr, int number_of_looser, int s)
    {
        for (int i = 0; i<s; i++)
        {
            test_array_of_permutations[number_of_looser][i] = arr[i];
        }
    }

    ///Zajisti, ze Offspring_2 < Offspring_1 (respektive jejich delky, je treba pro zjednoduseni vyhodnocovani nahrazeni loosers)
    void editOffsprings()
    {
        int pom;

        if (length_of_offspring_1 < length_of_offspring_2)
        {
            transformArrayAtoArrayB(array_of_transform, offspring_1, test_number_of_vertex);
            transformArrayAtoArrayB(offspring_1, offspring_2, test_number_of_vertex);
            transformArrayAtoArrayB(offspring_2, array_of_transform, test_number_of_vertex);

            pom = length_of_offspring_1;
            length_of_offspring_1 = length_of_offspring_2;
            length_of_offspring_2 = pom;
        }
    }

    ///tato funkce se da diky vlastnostem ofsprings (O2 < O1) a loosers (L2 < L1) upravit efektivneji, nicmene je to prehlednejsi takto, kdyz se ty podminky sepisou do
    ///tabulky tak je presne videt co se deje, pokud by se to udelalo co nejkratsi tak se v budoucnu nikdo v tom kodu nema sanci vyznat
    void replaceLoosersAndOffsprings()
    {
        ///L1 := O2
        if (((lenght_of_looser_2_Max2 < length_of_offspring_2) && (length_of_offspring_2 < lenght_of_looser_1_Max)) && (lenght_of_looser_1_Max < length_of_offspring_1))
        {
            transformLooserToOffspring(offspring_2, number_of_looser_1, test_number_of_vertex);
        }
        ///L2 := O2
        if (((length_of_offspring_2 < lenght_of_looser_2_Max2) && (lenght_of_looser_2_Max2 < lenght_of_looser_1_Max)) && (lenght_of_looser_1_Max < length_of_offspring_1))
        {
            transformLooserToOffspring(offspring_2, number_of_looser_2, test_number_of_vertex);
        }
        ///L2 := O2, L1 := O1
        if (((length_of_offspring_2 < length_of_offspring_1) && (length_of_offspring_1 < lenght_of_looser_2_Max2)) && (lenght_of_looser_2_Max2 < lenght_of_looser_1_Max))
        {
            transformLooserToOffspring(offspring_2, number_of_looser_2, test_number_of_vertex);
            transformLooserToOffspring(offspring_1, number_of_looser_1, test_number_of_vertex);
        }
        ///L2 := O2, L1 := O1
        if (((length_of_offspring_2 < lenght_of_looser_2_Max2) && (lenght_of_looser_2_Max2 < length_of_offspring_1)) && (length_of_offspring_1 < lenght_of_looser_1_Max))
        {
            transformLooserToOffspring(offspring_2, number_of_looser_2, test_number_of_vertex);
            transformLooserToOffspring(offspring_1, number_of_looser_1, test_number_of_vertex);
        }
        /// L1 := O2
        if(((lenght_of_looser_2_Max2 < length_of_offspring_2) && (length_of_offspring_2 < length_of_offspring_1)) && (length_of_offspring_1 < lenght_of_looser_1_Max))
        {
            transformLooserToOffspring(offspring_2, number_of_looser_1, test_number_of_vertex);
        }
    }

    void isOffspringBetterLoosers()
    {
        length_of_offspring_1 = lengthOfOffspring(offspring_1, test_number_of_vertex);
        length_of_offspring_2 = lengthOfOffspring(offspring_2, test_number_of_vertex);

        editOffsprings();

        replaceLoosersAndOffsprings();
    }

    ///duplikuje "dup_ee" na "dup_er" (v "-ee" bude "-er")
    void duplicate(int *dup_ee, int *dup_er, int s)
    {
        for (int i = 0; i<s; i++)
            dup_ee[i] = dup_er[i];
    }

    ///najde "polovinu" pole, ulozi do "cross_point"
    void findHalf(int number)
    {
        if ((number % 2) == 0)
        {
            cross_point = number / 2;
        }
        else
        {
            cross_point = (number - 1) / 2;
        }
    }

    ///v "arr" prohodi prvky na pozicich "a" & "b"
    void switchF(int *arr, int a, int b)
    {
        int pom = arr[a];
        arr[a] = arr[b];
        arr[b] = pom;
    }

    void printArray(int *arr, int s)
    {
        for (int i = 0; i<s; i++)
            printf("%d ", arr[i]);

        printf("\n");
    }

    void printOffsprings()
    {
        printf("offspring 1:\n");
        printArray(offspring_1, test_number_of_vertex);
        printf("length of offspring 1: %d\n", length_of_offspring_1);
        printf("offspring 2:\n");
        printArray(offspring_2, test_number_of_vertex);
        printf("length of offspring 2: %d\n", length_of_offspring_2);
    }

    ///crossing - prohazovani - probiha v arr_1
    void crossing(int *arr1, int *arr2, int s)
    {
        for (int i = 0; i<cross_point; i++)
        {
            for (int j = 0; j<s; j++)
            {
                if (arr1[j] == arr2[i])
                {
                    find_for = j;
                }
            }
            switchF(arr1, i, find_for);;
        }
    }

    void crossFunction()
    {
        findHalf(test_number_of_vertex);
        duplicate(offspring_1, parent_1, test_number_of_vertex);
        duplicate(offspring_2, parent_2, test_number_of_vertex);
        crossing(offspring_1, parent_2, test_number_of_vertex);
        crossing(offspring_2, parent_1, test_number_of_vertex);
    }
    /// ------------------------------------------END----------------------------------------------------------------


    /// ------------------------------------------BEGIN-HLAVNI-FUNKCE-------------------------------------------------------------
    void printActualOutput(int number_of_gen)
    {
        printf("Number of generation: %d\n", number_of_gen);
        printf("Actual best permutation: ");
        printArray(parent_1, test_number_of_vertex);
        printf("Length of actual best permutation: %d\n", lenght_of_parent_1_Min);
        printf("\n");

        printOffsprings();
    }

    void printFinalOutput(int number_of_gen)
    {
        printf("=====================================\n");
        printf("Number of final generation: %d\n", number_of_gen);
        printf("=====================================\n");
        printf("Final best permutation: ");
        printArray(parent_1, test_number_of_vertex);
        printf("Length of final best permutation: %d\n", lenght_of_parent_1_Min);
    }

    void TSP_pmx()
    {
        int i;

        createArrayOfNumberOfPermutations(test_number_of_vertex, test_number_of_start_permutations);
        createArrayOfPermutations();

        for (i = 0; i<test_number_of_generations; i++)
        {
            findParentsAndLoosers(test_number_of_start_permutations);
            crossFunction();
            isOffspringBetterLoosers();

            printActualOutput(i);
        }

        findParentsAndLoosers(test_number_of_start_permutations);
        printFinalOutput(i);
    }
    /// ------------------------------------------END----------------------------------------------------------------
};

int main()
{
    loadFileBase("C:\\data.txt");

    TravellingSalesmanProblem *TSP = new TravellingSalesmanProblem(test_number_of_vertex, test_number_of_start_permutations);

    TSP->loadFile("C:\\data.txt");
    TSP->TSP_pmx();

    return 0;
}

