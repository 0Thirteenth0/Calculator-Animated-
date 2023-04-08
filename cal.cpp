#include "cal.h"

bool special_characters(char c) {
    std::string s = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890`~!@#$%^&*()-_+[]{}\\|;:'\",<.>/?";
    for (unsigned int i = 0; i < s.length(); i++)
        if (s[i] == c) return false;
    return true;
}
std::string delete_last_char(std::string s) {
    std::string s1 = "";
    for (int i = 0; i < s.length() - 1; i++)
        s1 += s[i];
    return s1;
}
void printToCalScreen(std::string s) {
    std::string c = "\033[5;" + std::to_string(84 - s.length()) + "H";
    std::cout << c << s << std::endl;
}
void cursorSet(int y, int x) {
    std::string c = "\033[" + std::to_string(y) + ";" + std::to_string(x) + "H";
    std::cout << c;
}


//calculator calculation class
cal::cal() : answer("") {}
cal::~cal() {}
cal::cal(std::string user_input){
    for (unsigned int i = 0; i < user_input.length(); i++)
    {
        if (user_input[i] != ' ')
        {
            if (isNum(user_input[i]) || isSym(user_input[i]) || user_input[i] == '(' || user_input[i] == ')' || user_input[i] == '.')
            {
                problem += user_input[i];
                if (i + 1 < user_input.length() && isSym(user_input[i]) && isSym(user_input[i + 1]) && user_input[i+1] != '-')
                {
                    answer = "Incorrect Format!!!";
                    return;
                }
            }else{
                answer = "The program is unable to solve this problem! Please look in the help menu!";
                return;
            }
        }
    }
    unsigned int oP = open_parenthesis(problem), cP = close_parenthesis(problem);
    if (oP != cP)
    {
        answer = "Incorrect Format!!! >>>>>>>Missing Parenthesis Part<<<<<<<";
        return;
    }
    while (true)
    {
        unsigned int starting_index = findStartingCal(oP, problem);
        if (!oP)
            starting_index--;
        unsigned int indexing = starting_index;
        std::string cpy = "";
        double *num = nullptr;
        char *sym = nullptr;
        while (problem[indexing] != ')' && indexing < problem.length())
            cpy += problem[indexing++];
        int number_of_symbols = findNumActions(cpy);
        if (number_of_symbols >= findNumNumbers(cpy))
        {
            answer = "Incorrect Format! - Please Check Question Entered.";
            return;
        }
        num = new double [number_of_symbols + 1];
        sym = new char [number_of_symbols];
        initialize(cpy, num, sym);
        while (number_of_symbols)
        {
            int found_index = priorityAction(sym, number_of_symbols);
            double converted = 0;
            if (sym[found_index] == '/' && num[found_index] / num[found_index + 1] == 0)
            {
                answer = "Error: Division by Zero.";
                return;
            }
            else
            {
                converted = conversion(num[found_index], num[found_index + 1], sym[found_index]);
                vlueCHNGEarr(num, converted, sym, number_of_symbols, found_index);
            }
            number_of_symbols--;
        }
        if (!oP)
            answer = removePrecision(std::to_string(num[0]));
        problemEdit(removePrecision(std::to_string(num[0])), starting_index, indexing);
        delete[] num;
        delete[] sym;
        if (!oP)
            break;
        oP--;
        cP--;
    }
}
void cal::problemEdit(std::string converted, int starting_index, int ending_index) {
    std::string cpy = "";
    
    for (unsigned int i = 0; i < problem.length(); i++)
    {
        if (i < starting_index - 1 || i > ending_index)
        {
            cpy += problem[i];
        }
        else
        {
            cpy += converted;
            i = ending_index;
        }
        
    }
    problem = cpy;
}
std::string cal::removePrecision(std::string converted) {
    std::string cpy = "";
    int index_of_point = -1, reverse_traversal = converted.length();
    while (converted[++index_of_point] != '.') {}
    for(int i = 0; i < index_of_point; i++)
        cpy += converted[i];
    while (reverse_traversal > index_of_point && converted[reverse_traversal - 1] == '0') 
        reverse_traversal--;
    for (int i = reverse_traversal - index_of_point > 1 ? 0 : 1; i < reverse_traversal - index_of_point; i++)
        cpy += converted[i + index_of_point];
    return cpy;
}
void cal::vlueCHNGEarr(double *&num, double data_to_add, char *&sym, int num_of_symbols, int position_to_delete) {
    double *num_cpy = num;
    char *sym_cpy = sym;
    num = new double [num_of_symbols];
    sym = new char [num_of_symbols - 1];
    for (int i = 0; i < num_of_symbols; i++)
    {
        if (i < position_to_delete)
            num[i] = num_cpy[i];
        else if (i + 2 < num_of_symbols + 1)
            num[i + 1] = num_cpy[i + 2];
    }
    num[position_to_delete] = data_to_add;
    for (int i = 0; i < num_of_symbols - 1; i++)
    {
        if (i < position_to_delete)
            sym[i] = sym_cpy[i];
        else
            sym[i] = sym_cpy[i + 1];
    }
    delete[] num_cpy;
    delete[] sym_cpy;
    num_cpy = nullptr;
    sym_cpy = nullptr;
}
double cal::conversion(double &num1, double &num2, char sym) const {
    switch (sym)
    {
    case '+':
        return num1 + num2;
        break;
    case '-':
        return num1 - num2;
        break;
    case '*':
        return num1 * num2;
        break;
    case '/':
        return num1 / num2;
        break;
    case '^':
        return pow(num1,num2);
        break;
    }
    return -1;
}
int cal::priorityAction(char *sym, int number_of_symbols) const {
    if (number_of_symbols > 1)
    {
        int largest = -1;
        int found_index = 0;
        for (int i = 0; i < number_of_symbols ; i++)
        {
            if (largest < valueOfSym(sym[i]))
            {
                found_index = i;
                largest = valueOfSym(sym[i]);
            }
        }
        return found_index;
    }
    return 0;
}
int cal::valueOfSym(char sym) const {
    if (isSym(sym))
    {
        switch (sym)
        {
        case '+':
        case '-':
            return ADD;
            break;
        case '/':
        case '*':
            return MULT;
            break;
        case '^':
            return POWER;
            break;
        }
    }
    return -1;
}
void cal::initialize(std::string PROBLEM, double *num, char *sym) {
    std::string cpy = "";
    int indexing = 0;
    for (int i = 0; i < PROBLEM.length(); i++)
    {
       if (isSym(PROBLEM[i]))
       {
            num[indexing] = std::stod(cpy);
            sym[indexing++] = PROBLEM[i];
            cpy = "";
            if (i + 1 < PROBLEM.length() && PROBLEM[i + 1] == '-')
            {
                cpy += PROBLEM[++i];
            }
       }
       else 
       {
            cpy += PROBLEM[i];
       }
    }
    num[indexing] = std::stod(cpy);
    
}
int cal::findNumNumbers(std::string PROBLEM) const {
    int count = 0;
    for (unsigned int i = 0; i < PROBLEM.length(); i++)
    {
        if (isdigit(PROBLEM[i]))
        {
            count++;
            for (unsigned int j = i; j + 1 < PROBLEM.length(); j++)
            {
               if (isSym(PROBLEM[j + 1]))
               {
                    break;
               }
               i++;
            }
        }
    }
    return count;
}
int cal::findNumActions(std::string PROBLEM) const {
    int count = 0;
    for (unsigned int i = 0; i < PROBLEM.length(); i++)
    {
        if (isSym(PROBLEM[i]))
        {
            count++;
            if (PROBLEM[i + 1] == '-')
                i++;
        }
    }
    return count;
}
int cal::findStartingCal(int number_of_parenthesis, std::string PROBLEM) const{
    for (unsigned int i = 0; i < PROBLEM.length(); i++)
    {
        if (PROBLEM[i] == '(')
            number_of_parenthesis--;
        if (!number_of_parenthesis)
            return i + 1;
    }
    return -1;
}
std::string cal::getAns() const {
    return answer;
}
bool cal::isSym(char c) const {
    for (unsigned int i = 0; i < symbols.length(); i++)
        if (c == symbols[i])
            return true;
    return false;
}
bool cal::isNum(char c) const {
    for (unsigned int i = 0; i < Numbers.length(); i++)
        if (c == Numbers[i])
            return true;
    return false;
}
int cal::open_parenthesis(std::string PROBLEM) const {
    int count = 0;
    for (unsigned int i = 0; i < PROBLEM.length(); i++)
        if (PROBLEM[i] == '(')
            count++;
    return count;
}
int cal::close_parenthesis(std::string PROBLEM) const {
    int count = 0;
    for (unsigned int i = 0; i < PROBLEM.length(); i++)
        if (PROBLEM[i] == ')')
            count++;
    return count;
}

//color class
color::color() : reset("\u001b[0m"), cSize(16) {
    c = new std::string [cSize];
    bc = new std::string [cSize];
    int code = 0;
    for (int i = 0; i < cSize; i++)
    {
        if (code == 8)
            code = 0;
        if (i < cSize / 2)
            c[i] = "\u001b[3" + std::to_string(code++) + "m";
        else
            c[i] = "\u001b[3" + std::to_string(code++) + ";1m";
    }
    code = 0;
    for (int i = 0; i < cSize; i++)
    {
        if (code == 8)
            code = 0;
        if (i < cSize / 2)
            bc[i] = "\u001b[4" + std::to_string(code++) + "m";
        else
            bc[i] = "\u001b[4" + std::to_string(code++) + ";1m";
    }
}
color::~color() {
    delete[] c;
    delete[] bc;
    c = nullptr;
    bc = nullptr;
}
std::string color::getC(int x) const{
    return c[x];
}
std::string color::getBC(int x) const{
    return bc[x];
}
std::string color::cReset() const {
    return reset;
}
int color::getSize() const {
    return cSize;
}

//Calculator animation class
calMachine::calMachine() : cursorRst("\033[0;0H") {
    const int welSize = 5;
    std::string welcome [welSize] = {
                                        "  _      __    __                     __         __  __                  __         __     __           __",
                                        " | | /| / /__ / /______  __ _  ___   / /____    / /_/ /  ___   _______ _/ /_____ __/ /__ _/ /____  ____/ /",
                                        " | |/ |/ / -_) / __/ _ \\/  ' \\/ -_) / __/ _ \\  / __/ _ \\/ -_) / __/ _ `/ / __/ // / / _ `/ __/ _ \\/ __/_/ ",
                                        " |__/|__/\\__/_/\\__/\\___/_/_/_/\\__/  \\__/\\___/  \\__/_//_/\\__/  \\__/\\_,_/_/\\__/\\_,_/_/\\_,_/\\__/\\___/_/ (_)  ",
                                        "                                                                                                          "
                                    };
    srand(time(NULL));
    wScreen(welcome, welSize);
    system(CMD_MAX);
    system("clear");
    calculator();
}
calMachine::~calMachine() {}
void calMachine::wScreen(std::string s[], int size) {
    const int c_size = 14;
    std::string fade[c_size] = {
                                "\u001b[38;5;232m",
                                "\u001b[38;5;236m",
                                "\u001b[38;5;239m",
                                "\u001b[38;5;59m",
                                "\u001b[38;5;62m",
                                "\u001b[38;5;147m",
                                "\u001b[38;5;141m",
                                "\u001b[38;5;140m",
                                "\u001b[38;5;139m",
                                "\u001b[38;5;176m",
                                "\u001b[38;5;177m",
                                "\u001b[38;5;13m",
                                "\u001b[38;5;5m",
                                "\u001b[38;5;201m"
                                    };
    std::string dictionary = "01234567890!@#$%^&*";
    color p;
    std::string mark = "--By: thirteenth";
    const int width = 200, height = 50;
    int **effIndex = new int *[2];
    int count = 0, smallest = 9999, si = 0, efGroup = 2;
    for (int i = 0; i < 2; i++)
        effIndex[i] = new int [efGroup];
    system("clear");
    system(RESIZE_T);
    int x = 0, c = 0, length = 0, existGroup = 0;
    for (int i = 0; i < efGroup; i++)
        effIndex[0][i] = -1;
    for (int i = 0; i < efGroup; i++)
        effIndex[1][i] = 0;
    for (int i = 0; i < efGroup; i++)
    {
        do
            count = rand() % width;
        while (existing(effIndex[0],count));
        effIndex[0][i] = count;
    }
    count = 0;
    while (true)
    {
        int **temp;
        //check to add extra row of effect to the board
        if (existGroup > efGroup)
        {
            temp = new int *[2];
            for (int i = 0; i < 2; i++)
                temp[i] = new int [existGroup];
            //copy old elements to new array
            for (int i = 0; i < efGroup; i++)
            {
                temp[0][i] = effIndex[0][i];
                temp[1][i] = effIndex[1][i];
            }
            //initialize the newly added elements and give random index to newly added elements
            for (int i = 0; i < existGroup - efGroup; i++)
            {
                temp[0][i + efGroup] = -1;
                temp[1][i + efGroup] = 0;
            }
            for (int i = 0; i < existGroup - efGroup; i++)
            {
                do
                    si = rand() % width;
                while (existing(temp[0],si) || (si - temp[0][i + efGroup - 1] < 4  && si - temp[0][i + efGroup - 1] > -4));
                temp[0][i + efGroup] = si;
            }
            for (int i = 0; i < 2; i++)
                delete[] effIndex[i];
            delete[] effIndex;
            effIndex = temp;
            temp = nullptr;
            efGroup = existGroup;
            si = 0;
        }
        temp = new int *[2];
        for (int i = 0; i < 2; i++)
            temp[i] = new int [existGroup];
        x = 0, c = 0;
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < efGroup; j++)
            {
                temp[0][j] = effIndex[0][j];
                temp[1][j] = effIndex[1][j];
            }
            length = rand() % 5 == x && ((count > 15 && count < 30) || (count > 55 && count < 70))? s[x].length() / 2 + 3 : rand() % 5 == x && ((count > 15 && count < 30) || (count > 55 && count < 70)) ? (s[x].length() / 2) - 3 : s[x].length() / 2;
            for (int j = 0; j < width; j++)
            {    
                smallest = 9999;
                for (int p = 0; p < existGroup; p++)
                {
                    if (smallest > temp[0][p] && temp[0][p] >= j)
                    {
                        smallest = temp[0][p];
                        si = p;
                    }
                }
                if (i >= ((height / 2) - (size / 2)) && i < ((height / 2) + (size - size / 2)) && j == ((width / 2) - length))
                {
                    for (int g = 0; g < s[x].length(); g++)
                        std::cout << p.getC(5) << s[x][g] << p.cReset();
                    j += s[x++].length() - 1;
                    
                }
                else if (i == height - 1 && j > width - mark.length() - 1)
                {
                    std::cout << p.getC(5) << mark[c++] << p.cReset();
                }
                else if (temp[0][si] == j && i >= findEffEnd(temp[1][si], c_size) && i <= temp[1][si])
                {
                    std::cout << fade[starting_effect(temp[1][si], i, c_size)] << dictionary[rand() % dictionary.length()] << p.cReset();
                    temp[0][si] = 9999;
                }
                else
                {
                    std::cout << " ";
                }
            }  
            std::cout << std::endl;
        }
        si = efGroup;
        for (int i = 0; i < si; i++)
        {
            if (effIndex[1][i] > width - 1 + c_size)
            {
                remove_effect_element(effIndex, i, efGroup);
                existGroup = efGroup;
            }
            else
            {
                effIndex[1][i] += 2;
            }
        }
        for (int j = 0; j < 2; j++)
            delete[] temp[j];
        delete[] temp;
        temp = nullptr;
        if (!(count % 3))
            existGroup += 5;
        if(count++ > 80)
        {
            for (int i = 0; i < 2; i++)
                delete[] effIndex[i];
            delete[] effIndex;
            effIndex = nullptr;
            return;
        }
        std::cout << cursorRst;
        usleep(50000);
    }
}
void calMachine::calculator() {
    const int size = 32;
    std::string pic[size] = {
                            "  ___________________________________________________________________________________  ", 
                            "||                                     CASIO                                         ||",
                            "||___________________________________________________________________________________||",
                            "||                                                                                   ||",
                            "||                                                                                   ||",
                            "||                                                                                   ||",
                            "||___________________________________________________________________________________||",
                            "||___________________________________________________________________________________||",
                            "||                                                                                   ||",
                            "||      ___________       ___________       ___________         ______________       ||",
                            "||     |           |     |           |     |           |       |              |      ||",
                            "||     |    Del    |     |    +/-    |     |     ^     |       |      /       |      ||",
                            "||     |___________|     |___________|     |___________|       |______________|      ||",
                            "||      ___________       ___________       ___________         ______________       ||",
                            "||     |           |     |           |     |           |       |              |      ||",
                            "||     |     7     |     |     8     |     |     9     |       |      *       |      ||",
                            "||     |___________|     |___________|     |___________|       |______________|      ||",
                            "||      ___________       ___________       ___________         ______________       ||",
                            "||     |           |     |           |     |           |       |              |      ||",
                            "||     |     4     |     |     5     |     |     6     |       |      -       |      ||",
                            "||     |___________|     |___________|     |___________|       |______________|      ||",
                            "||      ___________       ___________       ___________         ______________       ||",
                            "||     |           |     |           |     |           |       |              |      ||",
                            "||     |     1     |     |     2     |     |     3     |       |      +       |      ||",
                            "||     |___________|     |___________|     |___________|       |______________|      ||",
                            "||      _____________________________       ___________         ______________       ||",
                            "||     |                             |     |           |       |              |      ||",
                            "||     |              0              |     |     .     |       |      =       |      ||",
                            "||     |_____________________________|     |___________|       |______________|      ||",
                            "||                                                                                   ||",
                            "||___________________________________________________________________________________||",
                            "|_____________________________________________________________________________________|",
                                };
    
    system("printf '\e[8;35;87t'");
    BufferToggle bt;
    int charASCII= 0;
    int x = 0, tick = 0;
    color calculator_color;
    std::string col = calculator_color.getC(5) + calculator_color.getBC(1);
    //calculator screen: 3-5, width: 83 (2 - 84)
    bt.off();
    std::string g = "", help_message = "Press ? for The Help Menu.";
    while (true)
    {   
        system("clear");
        char c;
        std::cout << cursorRst;
        for (int i = 0; i < size; i++)
        {
            for (unsigned int j = 0; j < pic[i].length(); j++)
            {
                if (i == 0 || j == 0 || j == pic[i].length() - 1)
                    std::cout << pic[i][j];
                else if (i >= 3 && i <= 6 && j > 1 && j < pic[i].length() - 2 )
                    std::cout << calculator_color.getBC(0) << pic[i][j] << calculator_color.cReset();
                else
                    std::cout << calculator_color.getBC(1) << pic[i][j] << calculator_color.cReset();
            }
            std::cout << std::endl;
        }
        std::cout << calculator_color.getBC(7) << calculator_color.getC(0) << std::setw(43 + help_message.length() / 2) << help_message << std::setw(44 - help_message.length() / 2) << " " << std::endl << calculator_color.cReset();
        while (tick++ < 10)
        {
            x = x == 0 ? 7 : 0;
            enterPScreen(x);
        }
        if (tick++ < 12)
            continue;
        if (charASCII != '\n' && charASCII != '=')
        {
            print_to_screen(g);
            charASCII = c = getchar();
            if (!special_characters(c))
                g += c;
            switch (c)
            {
            case '?':
                if (g.length() == 1)
                {
                    help_menu();
                    g = "";
                }
                break;
            case 127:
                if (g.length()) 
                    g = delete_last_char(g);
                break;
            default:
                break;
            } 
            if (is_cal_key(c))
                highlight_key(c);
        }
        else 
        {
            if (g.length() > 0){
                cal findAns(g);
                std::string the_answer = findAns.getAns();
                print_to_screen(the_answer);
            }
            g = "";
            charASCII = c = getchar();
        }
        
        usleep(50000);
    }
    bt.on();

}
void calMachine::highlight_key(char c) {
    color p;
    std::string col = p.getC(5) + p.getBC(1);
    switch (c)
    {
    case 127:
        cursorSet(10,8);
        std::cout << col << " ___________ " << p.cReset() << std::endl;
        cursorSet(11,8);
        std::cout << col << "|           |" << p.cReset() << std::endl;
        cursorSet(12,8);
        std::cout << col << "|    Del    |" << p.cReset() << std::endl;
        cursorSet(13,8);
        std::cout << col << "|___________|" << p.cReset() << std::endl;
        break;
    case '1':
        cursorSet(22,8);
        std::cout << col << " ___________ " << p.cReset() << std::endl;
        cursorSet(23,8);
        std::cout << col << "|           |" << p.cReset() << std::endl;
        cursorSet(24,8);
        std::cout << col << "|     1     |" << p.cReset() << std::endl;
        cursorSet(25,8);
        std::cout << col << "|___________|" << p.cReset() << std::endl;
        break;
    case '2':
        cursorSet(22,26);
        std::cout << col << " ___________ " << p.cReset() << std::endl;
        cursorSet(23,26);
        std::cout << col << "|           |" << p.cReset() << std::endl;
        cursorSet(24,26);
        std::cout << col << "|     2     |" << p.cReset() << std::endl;
        cursorSet(25,26);
        std::cout << col << "|___________|" << p.cReset() << std::endl;
        break;
    case '3':
        cursorSet(22,44);
        std::cout << col << " ___________ " << p.cReset() << std::endl;
        cursorSet(23,44);
        std::cout << col << "|           |" << p.cReset() << std::endl;
        cursorSet(24,44);
        std::cout << col << "|     3     |" << p.cReset() << std::endl;
        cursorSet(25,44);
        std::cout << col << "|___________|" << p.cReset() << std::endl;
        break;
    case '4':
        cursorSet(18,8);
        std::cout << col << " ___________ " << p.cReset() << std::endl;
        cursorSet(19,8);
        std::cout << col << "|           |" << p.cReset() << std::endl;
        cursorSet(20,8);
        std::cout << col << "|     4     |" << p.cReset() << std::endl;
        cursorSet(21,8);
        std::cout << col << "|___________|" << p.cReset() << std::endl;
        break;
    case '5':
        cursorSet(18,26);
        std::cout << col << " ___________ " << p.cReset() << std::endl;
        cursorSet(19,26);
        std::cout << col << "|           |" << p.cReset() << std::endl;
        cursorSet(20,26);
        std::cout << col << "|     5     |" << p.cReset() << std::endl;
        cursorSet(21,26);
        std::cout << col << "|___________|" << p.cReset() << std::endl;
        break;
    case '6':
        cursorSet(18,44);
        std::cout << col << " ___________ " << p.cReset() << std::endl;
        cursorSet(19,44);
        std::cout << col << "|           |" << p.cReset() << std::endl;
        cursorSet(20,44);
        std::cout << col << "|     6     |" << p.cReset() << std::endl;
        cursorSet(21,44);
        std::cout << col << "|___________|" << p.cReset() << std::endl;
        break;
    case '7':
        cursorSet(14,8);
        std::cout << col << " ___________ " << p.cReset() << std::endl;
        cursorSet(15,8);
        std::cout << col << "|           |" << p.cReset() << std::endl;
        cursorSet(16,8);
        std::cout << col << "|     7     |" << p.cReset() << std::endl;
        cursorSet(17,8);
        std::cout << col << "|___________|" << p.cReset() << std::endl;
        break;
    case '8':
        cursorSet(14,26);
        std::cout << col << " ___________ " << p.cReset() << std::endl;
        cursorSet(15,26);
        std::cout << col << "|           |" << p.cReset() << std::endl;
        cursorSet(16,26);
        std::cout << col << "|     8     |" << p.cReset() << std::endl;
        cursorSet(17,26);
        std::cout << col << "|___________|" << p.cReset() << std::endl;
        break;
    case '9':
        cursorSet(14,44);
        std::cout << col << " ___________ " << p.cReset() << std::endl;
        cursorSet(15,44);
        std::cout << col << "|           |" << p.cReset() << std::endl;
        cursorSet(16,44);
        std::cout << col << "|     9     |" << p.cReset() << std::endl;
        cursorSet(17,44);
        std::cout << col << "|___________|" << p.cReset() << std::endl;
        break;
    case '0':
        cursorSet(26,8);
        std::cout << col << " _____________________________ " << p.cReset() << std::endl;
        cursorSet(27,8);
        std::cout << col << "|                             |" << p.cReset() << std::endl;
        cursorSet(28,8);
        std::cout << col << "|              0              |" << p.cReset() << std::endl;
        cursorSet(29,8);
        std::cout << col << "|_____________________________|" << p.cReset() << std::endl;
        break;
    case '+':
        cursorSet(22,64);
        std::cout << col << " ______________ " << p.cReset() << std::endl;
        cursorSet(23,64);
        std::cout << col << "|              |" << p.cReset() << std::endl;
        cursorSet(24,64);
        std::cout << col << "|      +       |" << p.cReset() << std::endl;
        cursorSet(25,64);
        std::cout << col << "|______________|" << p.cReset() << std::endl;
        break;
    case '-':
        cursorSet(18,64);
        std::cout << col << " ______________ " << p.cReset() << std::endl;
        cursorSet(19,64);
        std::cout << col << "|              |" << p.cReset() << std::endl;
        cursorSet(20,64);
        std::cout << col << "|      -       |" << p.cReset() << std::endl;
        cursorSet(21,64);
        std::cout << col << "|______________|" << p.cReset() << std::endl;
        break;
    case '/':
        cursorSet(10,64);
        std::cout << col << " ______________ " << p.cReset() << std::endl;
        cursorSet(11,64);
        std::cout << col << "|              |" << p.cReset() << std::endl;
        cursorSet(12,64);
        std::cout << col << "|      /       |" << p.cReset() << std::endl;
        cursorSet(13,64);
        std::cout << col << "|______________|" << p.cReset() << std::endl;
        break;
    case '^':
        cursorSet(10,44);
        std::cout << col << " ___________ " << p.cReset() << std::endl;
        cursorSet(11,44);
        std::cout << col << "|           |" << p.cReset() << std::endl;
        cursorSet(12,44);
        std::cout << col << "|     ^     |" << p.cReset() << std::endl;
        cursorSet(13,44);
        std::cout << col << "|___________|" << p.cReset() << std::endl;
        break;
    case '\n':
    case '=':
        cursorSet(26,64);
        std::cout << col << " ______________ " << p.cReset() << std::endl;
        cursorSet(27,64);
        std::cout << col << "|              |" << p.cReset() << std::endl;
        cursorSet(28,64);
        std::cout << col << "|      =       |" << p.cReset() << std::endl;
        cursorSet(29,64);
        std::cout << col << "|______________|" << p.cReset() << std::endl;
        break;
    case '.':
        cursorSet(26,44);
        std::cout << col << " ___________ " << p.cReset() << std::endl;
        cursorSet(27,44);
        std::cout << col << "|           |" << p.cReset() << std::endl;
        cursorSet(28,44);
        std::cout << col << "|     .     |" << p.cReset() << std::endl;
        cursorSet(29,44);
        std::cout << col << "|___________|" << p.cReset() << std::endl;
        break;
    case '*':
        cursorSet(14,64);
        std::cout << col << " ______________ " << p.cReset() << std::endl;
        cursorSet(15,64);
        std::cout << col << "|              |" << p.cReset() << std::endl;
        cursorSet(16,64);
        std::cout << col << "|      *       |" << p.cReset() << std::endl;
        cursorSet(17,64);
        std::cout << col << "|______________|" << p.cReset() << std::endl;
        break;
    default:
        break;
    }
    cursorSet(33,87);
    std::cout << std::endl;
}
void calMachine::print_to_screen(std::string &s) {
    color c;
    std::string error = "Error: Too Many Characters!";
    std::string mov_curs = "\033[5;";
    mov_curs += s.length() > 80 ? std::to_string(84 - error.length()) : std::to_string(84 - s.length());
    mov_curs += "H";
    mov_curs += s.length() > 80 ? c.getC(1) + error + c.cReset() : s; 
    if (s.length() > 80) s = "";
    
    std::cout << mov_curs << std::endl;
    cursorSet(33,87);
    std::cout << std::endl;
}
bool calMachine::is_cal_key(char key) {
    char c = 127;
    std::string s = "1234567890+-*/^.=";
    s += c;
    for (unsigned int i = 0; i < s.length(); i++)
        if (key == s[i])
            return true;
    return false;
}
void calMachine::help_menu() {
    const int s_size = 9;
    system("printf '\e[8;25;100t'");
    system("clear");
    std::string s[s_size] = {
                    "     .-\"\"\"\"\"\"-.     ",
                    "   .'          '.   ",
                    "  /   O    -=-   \\  ",
                    " :                : ",
                    " |                | ",
                    " : ',          ,' : ",
                    "  \\  '-......-'  /  ",
                    "   '.          .'   ",
                    "     '-......-'     "
                    };
    color c;
    std::cout << cursorRst;
    std::cout << c.getBC(7) << c.getC(0) << std::setw(54) << std::right << "Help Menu" << std::setw(46) << " " << c.cReset() << std::endl;
    std::cout << c.getC(1) << std::setw(32) << "WARNING:" << c.getC(4) << " This Calculator ONLY works with basic math.\n" << c.cReset();
    std::cout << c.getC(5) << "   This program is capable of looking for parenthesis \"()\" to determine the order of operation.\n" << c.cReset();
    std::cout << c.getC(6) << "\t  Works with:\n\t  a. Addition(+)\n\t  b. Subtraction(-)\n\t  c. Multiplication(*)\n\t  d. Division(/)\n\t  e. Exponential(^)\n" << c.cReset();
    std::cout << std::endl << c.getC(5) << "   This program/calculator is made for fun. Code for this program may be messy and ugly.\n \t\t\t\t      please do not judge :)\n" << c.cReset();
    for (int i = 0; i < s_size; i++)
        std::cout << c.getC(3) << std::setw(68 - (s[i].length() / 2)) << s[i] << std::endl << c.cReset();
    std::cout << std::endl;
    std::cout << c.getC(1) << c.getBC(7) << std::setw(63) << "    Press ANY Key to Continue." << std::setw(37) << "" << std::endl << c.cReset();

    char p = getchar();
    system("printf '\e[8;35;87t'");
    system("clear");
    
}
void calMachine::enterPScreen(int x) {
    color p;
    const int mSize = 3;
    std::string msg[mSize] = {
                                "+-+-+-+-+-+-+ +-+-+-+-+-+ +-+ +-+-+-+-+-+-+-+-+",
                                "|P|l|e|a|s|e| |e|n|t|e|r| |a| |p|r|o|b|l|e|m|.|",
                                "+-+-+-+-+-+-+ +-+-+-+-+-+ +-+ +-+-+-+-+-+-+-+-+"
                                };
    //calculator screen: 4-6, width: 83 (2 - 84)
    std::cout << "\033[4;20H";
    std::cout << p.getC(x) << msg[0] << p.cReset() << std::endl;
    std::cout << "\033[5;20H";
    std::cout << p.getC(x) << msg[1] << p.cReset() << std::endl;
    std::cout << "\033[6;20H";
    std::cout << p.getC(x) << msg[2] << p.cReset() << std::endl;
    cursorSet(33,87);
    std::cout << std::endl;
    usleep(150000);
}
bool calMachine::existing(int indexes[], int number) {
    int i = 0;
    while (indexes[i] != -1)
        if (number == indexes[i++])
            return true;
    return false;
}
//returns the fade index that corresponds with the current index of matrix on the board.
int calMachine::starting_effect(int starting_index, int current_index, int effect_size) {
    return effect_size - (starting_index - current_index) - 1;
}
//returns the starting point of the fade/matrix.
int calMachine::findEffEnd(int starting_index, int effect_size) {
    return starting_index > effect_size - 1 ? starting_index - effect_size + 1 : 0;
}
void calMachine::remove_effect_element(int **arr, int index_delete, int &size) {
    int **temp = new int *[2];
    for (int i = 0; i < 2; i++)
        temp[i] = new int [size - 1];
    int c = 0;
    for (int i = 0; i < size; i++)
    {
        if(index_delete != i)
        {
            temp[0][c] = arr[0][i];
            temp[1][c++] = arr[1][i];
        }
    }
    for (int i = 0; i < 2; i++)
        delete[] arr[i];
    delete[] arr;
    arr = temp;
    temp = nullptr;
    size--;
}