//Owen Youngblood
//This code automates a game of superbal
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath> //for distance formula functions
#include "disjoint.h"

using namespace std;

#define talloc(type, num) (type *) malloc(sizeof(type)*(num))

class Superball {
public:
    Superball(int argc, char **argv);
    int r;
    int c;
    int mss;
    int empty;
    vector<int> board;
    vector<int> goals;
    vector<int> colors;
    void analyze_superball();
};

void usage(const char *s) {
    fprintf(stderr, "usage: sb-read rows cols min-score-size colors\n");
    if (s != NULL) fprintf(stderr, "%s\n", s);
    exit(1);
}

Superball::Superball(int argc, char **argv) {
    int i, j;
    string s;

    if (argc != 5) usage(NULL);

    if (sscanf(argv[1], "%d", &r) == 0 || r <= 0) usage("Bad rows");
    if (sscanf(argv[2], "%d", &c) == 0 || c <= 0) usage("Bad cols");
    if (sscanf(argv[3], "%d", &mss) == 0 || mss <= 0) usage("Bad min-score-size");

    colors.resize(256, 0);

    for (i = 0; i < strlen(argv[4]); i++) {
        if (!isalpha(argv[4][i])) usage("Colors must be distinct letters");
        if (!islower(argv[4][i])) usage("Colors must be lowercase letters");
        if (colors[argv[4][i]] != 0) usage("Duplicate color");
        colors[argv[4][i]] = 2 + i;
        colors[toupper(argv[4][i])] = 2 + i;
    }

    board.resize(r * c);
    goals.resize(r * c, 0);

    empty = 0;

    for (i = 0; i < r; i++) {
        if (!(cin >> s)) {
            fprintf(stderr, "Bad board: not enough rows on standard input\n");
            exit(1);
        }
        if (s.size() != c) {
            fprintf(stderr, "Bad board on row %d - wrong number of characters.\n", i);
            exit(1);
        }
        for (j = 0; j < c; j++) {
            if (s[j] != '*' && s[j] != '.' && colors[s[j]] == 0) {
                fprintf(stderr, "Bad board row %d - bad character %c.\n", i, s[j]);
                exit(1);
            }
            board[i * c + j] = s[j];
            if (board[i * c + j] == '.') empty++;
            if (board[i * c + j] == '*') empty++;
            if (isupper(board[i * c + j]) || board[i * c + j] == '*') {
                goals[i * c + j] = 1;
                board[i * c + j] = tolower(board[i * c + j]);
            }
        }
    }
}

void Superball::analyze_superball() 
{
    //as hinted by zach in the Lab-questions discord chat
    DisjointSetByRankWPC *ds = new DisjointSetByRankWPC(r * c);
    for (int i = 0; i < r * c; i++) 
    {
        ds->Find(i);
    }
    //Step 1: union adjacent colors (as hinted by zach in the Lab-questions discord chat)
    for (int i = 0; i < r; i++) 
    {
        for (int j = 0; j < c; j++) 
	{
            int index = i * c + j;
            //Check right cell
            if (j + 1 < c && board[index] == board[index + 1] && board[index] != '.' && board[index] != '*') 
	    {
                int setIdLeft = ds->Find(index);
                int setIdRight = ds->Find(index + 1);
                if (setIdLeft != setIdRight) 
		{
                    ds->Union(setIdLeft, setIdRight);
                }
            }
            //Check bottom cell
            if (i + 1 < r && board[index] == board[index + c] && board[index] != '.' && board[index] != '*') 
	    {
                int setIdLeft = ds->Find(index);
                int setIdRight = ds->Find(index + c);
                if (setIdLeft != setIdRight) 
		{
                    ds->Union(setIdLeft, setIdRight);
                }
            }
        }
    }
    //Step 2: Calculate sizes of all components and track scoring cells
    vector<int> component_sizes(r * c, 0);
    vector<int> goal_rows(r * c, -1); //Row indices of goal cells
    vector<int> goal_cols(r * c, -1); //Column indices of goal cells
    for (int i = 0; i < r * c; i++) {
        if (board[i] != '.' && board[i] != '*') {
            int root = ds->Find(i);
            component_sizes[root]++;
            if (goals[i] == 1) {
                goal_rows[root] = i / c;
                goal_cols[root] = i % c;
            }
        }
    }
    //Step 3: Find the largest scoring set (both above and below mss)
    int bestRoot = -1;
    int bestRootUnderMSS = -1;
    int maxSizeAboveMSS = 0;
    int maxSizeUnderMSS = 0;
    for (int i = 0; i < r * c; i++) 
    {
        if (goals[i] == 1) 
	{
            int root = ds->Find(i);
            int size = component_sizes[root];
            if (size >= mss && size > maxSizeAboveMSS) 
	    {
                bestRoot = root;  //The largest set that is greater than or equal to mss
                maxSizeAboveMSS = size;
            } 
	    else if (size < mss && size > maxSizeUnderMSS)
	    {
                bestRootUnderMSS = root;  //The largest set that is smaller than mss
                maxSizeUnderMSS = size;
            }
        }
    }
    //Step 4: Score if a valid set larger than mss is found
    if (bestRoot != -1 && maxSizeAboveMSS >= mss) 
    {
        cout << "SCORE " << goal_rows[bestRoot] << " " << goal_cols[bestRoot] << endl;
    }
    //Step 5: If no valid set larger than mss, focus on the largest set under mss
    if (bestRootUnderMSS != -1) 
    {
        bestRoot = bestRootUnderMSS;
    }
    //swap two non-empty cells if there are no occupied scoring cells
    if (bestRoot == -1) 
    {
        int firstNonEmptyCell = -1;
        int secondNonEmptyCell = -1;
        for (int i = 0; i < r * c; i++) 
        {
            if (board[i] != '.' && board[i] != '*') 
	    {
                if (firstNonEmptyCell == -1) 
		{
                    firstNonEmptyCell = i;
                } else if (secondNonEmptyCell == -1) 
		{
                    secondNonEmptyCell = i;
                    break; //found two non-empty cells
                }
            }
        }
        if (firstNonEmptyCell != -1 && secondNonEmptyCell != -1) 
	{
            cout << "SWAP " << firstNonEmptyCell / c << " " << firstNonEmptyCell % c << " "
                 << secondNonEmptyCell / c << " " << secondNonEmptyCell % c << endl;
        }
    }
    //Step 6: Find the closest non-set cell with a different value
    int closestNonSetCell = -1;
    double minDistance = r*c;
    for (int i = 0; i < r; i++) 
    {
        for (int j = 0; j < c; j++) 
	{
            int index = i * c + j;
            if (ds->Find(index) != bestRoot && board[index] != '.' && board[index] != '*' && board[index] != board[bestRoot]) 
	    {
                double distance = sqrt(pow(i - goal_rows[bestRoot], 2) + pow(j - goal_cols[bestRoot], 2)); //distance formula
                if (distance < minDistance) 
		{
                    minDistance = distance;
                    closestNonSetCell = index;
                }
            }
        }
    }
    //Step 7: Check for a cell with the same value as the set outside the set
    int sameValueCell = -1;
    for (int i = 0; i < r * c; i++) 
    {
        if (ds->Find(i) != bestRoot && board[i] == board[bestRoot]) 
	{
            sameValueCell = i;
            break; //Take the first matching cell
        }
    }
    //Swap two cells if there are no valid cells with the same value as the set outside the set
    if (sameValueCell == -1) 
    {
        int firstNonEmptyCell = -1;
        int secondNonEmptyCell = -1;
        for (int i = 0; i < r * c; i++) 
	{
            if (board[i] != '.' && board[i] != '*') 
	    {
                if (firstNonEmptyCell == -1) 
		{
                    firstNonEmptyCell = i;
                } else if (secondNonEmptyCell == -1) 
		{
                    secondNonEmptyCell = i;
                    break; //found two non-empty cells
                }
            }
        }
        if (firstNonEmptyCell != -1 && secondNonEmptyCell != -1) 
	{
            cout << "SWAP " << firstNonEmptyCell / c << " " << firstNonEmptyCell % c << " "
                 << secondNonEmptyCell / c << " " << secondNonEmptyCell % c << endl;
        }
    }
    //Step 8: Print the swap command if valid cells found
    if (closestNonSetCell != -1 && sameValueCell != -1) 
    {
        cout << "SWAP " << closestNonSetCell / c << " " << closestNonSetCell % c << " "
             << sameValueCell / c << " " << sameValueCell % c << endl;
    }
}

int main(int argc, char **argv) 
{
    Superball *s;
    s = new Superball(argc, argv);
    s->analyze_superball();
    delete s;
    return 0;
}

