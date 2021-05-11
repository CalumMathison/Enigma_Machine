//https://www.codeproject.com/Articles/831015/ENIGMA

#include <iostream>

using namespace std;

#define Nchars 69
#define Nchars 70
#define Nrotors 11
#define Nrefls 5
#define Nsteps 11

char* ROTOR[Nrotors]
= {
    //Input alphabet
    "abcdefghijklmnopqrstuvwxyz.,:; ()[]'\"-+/*&~`!@#$%^_={}|\\<>?",
    // rotor 1
    "ekmflgdqvzntowyhxuspaibrcj4.:5,63)-&;' +*7/\"](081[29?><\\|}{=^_%$#@!`~",
    // rotor 2
    "ajdksiruxblhwtmcqgznpyfvoe093.]8[\"/1,7+':2)6&;(*5- 4?><\\|}{=^_%$#@!`~",
    // rotor 3
    "bdfhjlcprtxvznyeiwgakmusqo13579,2(['/-&;*48+60.:\"]) ?><\\|}{=^_%$#@!`~",
    // rotor 4
    "esovpzjayquirhxlnftgkdcmwb4] -(&90*)\"8[7/,;5'6.32:+1?><\\|}{=^_%$#@!`~",
    // rotor 5
    "vzbrgityupsdnhlxawmjqofeck-&1[68'*\"(]3;7,/0+:9) 542.?><\\|}{=^_%$#@!`~",
    // rotor 6
    "jpgvoumfyqbenhzrdkasxlictw9(6- \":5*)14;7&[3.0]/,82'+?><\\|}{=^_%$#@!`~",
    // rotor 7
    "nzjhgrcxmyswboufaivlpekqdt;&976[2/:*]+1 \"508-,(4.)3'?><\\|}{=^_%$#@!`~",
    // rotor 8
    "fkqhtlxocbjspdzramewniuygv5.)7',/ 219](3&[0:4+;8\"*6-?><\\|}{=^_%$#@!`~",
    // beta rotor
    "leyjvcnixwpbqmdrtakzgfuhos,4*9-2;8/+(1):3['0.&65\"7 ]?><\\|}{=^_%$#@!`~",
    // gamma rotor
    "fsokanuerhmbtiycwlqpzxvgjd5] .0;\"4[7:1'8*2+,)(&/-693?><\\|}{=^_%$#@!`~"
};

char NOTCH[Nrotors]
= { 'z', 'q', 'e', 'v', 'j', 'z', 'z', 'z', 'z', 'a', 'a' };

char* REFLECTOR[Nrefls]  // Reflectors
= {
    // input alphabet ("REFLECTOR" 0, not used)
    "abcdefghijklmnopqrstuvwxyz0123456789.,:; ()[]'\"-+/*&~`!@#$%^_={}|\\<>?",
    // reflector B, thick
    "yruhqsldpxngokmiebfzcwvjat*[\"7)],3(/;6 .:8415&2+-90'?<>\\|}{=_^%$#@`!~",
    // reflector C, thick
    "fvpjiaoyedrzxwgctkuqsbnmhl5-(980 *43[&/+62'.\")]1;:7,?<>\\|}{=_^%$#@`!~",
    // reflector B, dunn
    "enkqeuywjicopblmdxzvfthrgs4;.)0\"*+982 (1,:3/&-5'7[6]?<>\\|}{=_^%$#@`!~",
    // reflector C, dunn
    "rdobjntkvehmlfcwzrxgyipsuq[3 19;'.-47:,52+&0/6*8(]\")?<>\\|}{=_^%$#@`!~"
};

char* PLUGBOARD  // Default wirings of the plugboard
=
"abcdefghijklmnopqrstuvwxyz0123456789.,:; ()[]'\"-+/*&~`!@#$%^_={}|\\<>?";

char* alphabet  // Input alphabet
=
"abcdefghijklmnopqrstuvwxyz0123456789.,:; ()[]'\"-+/*&~`!@#$%^_={}|\\<>?";

int mRotors, mSteps;
int RotPos[Nrotors];
char window[Nrotors], Iwindow[Nrotors];
char* RotWiring[Nrotors];
char RotNotch[Nrotors];
int RotNumber[Nrotors];
char* reflector, plugboard[Nchars];
int ReflType;
char step[Nsteps];

#define Nline 255

FILE* inFp, * outFp, * logFp;
char inLine[Nline], outline[Nline];

int main()
{
    InitEnigma();
    TryUserSetup();

    ProcessFile("plain", "encrypt", "elog");
    Reset();
    ProcessFile("encrypt", "decrypt", "dlog");
}

void InitEnigma()
{
    int i;
    mRotors = 3;
    mSteps = (mRotors << 1) + 3;
    strcpy(plugboard, PLUGBOARD);
    for (int i = 0; i <= mRotors; ++i)
    {
        RotWiring[i] = ROTOR[i];
        RotNotch[i] = NOTCH[i];
        RotNumber[i] = i;
        Iwindow[i] = window[i] = 'a';
    }
    reflector = REFLECTOR[1];
    ReflType = 1;
}

void TryUserSetup()
{
    if ((inFp = fopen("esetup", "rt")) != NULL)
    {
        SetPlugboard();
        SetRotorsAndReflectors();
        fclose(inFp);
    }
}

void ProcesFile(char* inFname, char* encFname, char* logFname)
{
    if (OpenFiles(inFname, encFname, logFname))
    {
        SetRotorPositions();
        ReportMachine();
        ProcessPlainText();
        CloseFiles();
    }
}

void Reset()
{
    for (int i = 1; i <= mRotors; ++i)
    {
        window[i] = Iwindow[i];
    }
}

int OpenFiles(char* inFname, char* encFname, char* logFname)
{
    inFp = fopen(inFname, "rt");
    outFp = fopen(encFname, "wt");
    logFp = fopen(logFname, "wt");
    return (inFp != NULL) && (outFp != NULL) && (logFp != NULL);
}

void CloseFiles()
{
    fclose(inFp); fclose(outFp); fclose(logFp);
}

void ReportMachine()
{
    fprintf(logFp, "Plugboard mappings:\n");
    fprintf(logFp, "%s\n", ROTOR[0]);
    fprintf(logFp, "%s\n", plugboard);

    fprintf(logFp, "\nRotor wirings:\n ");
    fprintf(logFp, "position rotor ring setting notch sequence\n");
    for (int i = mRotors; i >= 1; --i)
    {
        fprintf(logFp, "%8d %5d %12c %5c %s\n",
            i, RotNumber[i], window[i],
            RotNotch[i], RotWiring[i]);
    }
    fprintf(logFp, "\nreflector %c %s\n", ReflType, reflector);
    fprintf(logFp, "\nrotors: \n");
    ShowRotors();
}

void ShowRotors()
{
    int i, j, k;
    char* Rwiring;

    for (i = mRotors; i >= 1; --i)
    {
        fprintf(logFp, "%d: ", i);
        Rwiring = RotWiring[i];
        k = RotPos[i];
        for (j = 0; j < k; ++j)
        {
            fprintf(logFp, "%c", *Rwiring++);
        }
        fprintf(logFp, "->");
        for (j = k; j < Nchars; ++j)
        {
            fprintf(logFp, "%c", *Rwiring++);
        }
        fprintf(logFp, "\n");
    }
}

void SetPlugboard()
{
    int i, n, x;
    char p1, p2, ch;

    fgets(inLine, Nline, inFp);
    inLine[strlen(inLine) - 1] = '\0';
    n = strlen(inLine);

    for (i = 0; i < n; i+=2)
    {
        p1 = inLine[i];
        p2 = inLine[i + 1];
        x = index(p1);
        if ((ch = plugboard[x]) != p1)
        {
            plugboard[index(ch)] = ch;
            plugboard[x] = p1;
        }
        plugboard[x] = p2;
        x = index(p2);
        if ((ch = plugboard[x]) != p2)
        {
            plugboard[index(ch)] = ch;
            plugboard[x] = p1;
        }
        plugboard[x] = p1;
    }
}

void SetRotorsAndReflector()
{
    int i, n, rotor, rotPos;
    char ch, ringPos;

    fgets(inLine, Nline, inFp);
    mRotors = ChrToInt(inLine[0]);
    if (mRotors > 4)
    {
        mRotors = 4;
    }
    mSteps = (mRotors << 1) + 3;
    for (i = 1; i <= mRotors; ++i)
    {
        fgets(inLine, Nline, inFp);
        ch = inLine[0];
        if (isdigit((int)ch))
        {
            rotor = ChrToInt(ch);
        }
        else
        {
            ch = tolower(ch);
            rotor = ch == 'b' ? 9 : ch == 'g' ? 10 : 0;
        }
        rotPos = ChrToInt(inLine[1]);
        ringPos = inLine[2];
        Iwindow[rotPos] = window[rotPos] = ringPos;
        PlaceRotor(rotPos, rotor);
    }

    fgets(inLine, Nline, inFp);
    ch = inLine[0];
    switch (ch)
    {
    case 't': n = 0; break;
    case 'b': n = 1; break;
    case 'c': n = 2; break;
    case 'B': n = 3; break;
    case 'C': n = 4; break;
    default: n = 0; break;
    }
    reflector = REFLECTOR[n];
    ReflType = 1;
}

int index(char c)
{
    int i = 0;
    while ((i < Nchars) && (c != alphabet[i]))
    {
        ++i;
    }
    return i;
}

int ChrToInt(char c)
{
    return (int)(c - '0');
}

void PlaceRotor(int position, int r)
{
    RotWiring[position] = ROTOR[r];
    RotNotch[position] = NOTCH[r];
    RotNumber[position] = r;
}

void SetRotorPositions()
{
    int i, j, k;
    char* Rwiring, ch;
    for (i = 1; i <= mRotors; ++i)
    {
        j = RotNumber[i];
        ch = window[j];
        Rwiring = RotWiring[j];
        k = 0;
        while (Rwiring[k] != ch)
        {
            ++k;
        }
        RotPos[j] = k;
    }
}

int mod(int n, int modulus)
{
    while (n >= modulus)
    {
        n -= modulus;
    }
    while (n < 0)
    {
        n += modulus;
    }
    return n;
}

void ProcessPlainText()
{
    int i, n;
    char c1, c2;
    
    fprintf(logFp, "\n\nEncryption\n");
    while (fgets(inLine, Nline, inFp) != NULL)
    {
        n = strlen(inLine) - 1;
        inLine[n] = '\0';

        for (i = 0; i < n; ++i)
        {
            c1 = inLine[i];
            if (isupper((int)c1))
            {
                c1 = tolower(c1);
            }

            c2 = encrypt(c1);

            ShowWindow();
            fprintf(logFp, "%c", c1);
            ShowSteps();
            fprintf(logFp, "\n");
            outLine[i] = c2;
        }

        fprintf(logFp, "\n");
        outLine[i] = '\0';
        fprintf(outFp, "%s\n" outLine);
    }
}

void ShowWindow()
{
    int i;

    for (i = mRotors; i >= 1; --i)
    {
        fprintf(logFp, "%c ", window[i]);
    }
    fprintf(logFp, " ");
}

void ShowSteps()
{
    int i;

    for (i = 0; i < mSteps; ++i)
    {
        fprintf(logFp, " -> %c", step[i]);
    }
}

char Encrypt(char c)
{
    int i, r;

    Turn();
    i = 0;
    step[i++] = plugboard[index(c)];
    for (r = 1; r <= mRotors; ++r)
    {
        steps[i++] = reflector(step[i - 1], r);
    }
}