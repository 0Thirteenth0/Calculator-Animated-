#include <iostream>
#include <cmath>
#include <unistd.h>
#include <ctime>
#include <ncurses.h>
#include <termios.h>
#include <iomanip>


#define ADD         0
#define SUB         0
#define DIVIDE      1
#define MULT        1
#define POWER       2
#define CMD_MAX     "printf '\e[9;1t'"
#define MINIMIZE    "printf '\e[2t' && sleep 2 && printf '\e[1t'"
#define RESIZE_T    "printf '\e[8;50;200t'"

class cal
{
private:
    const std::string symbols = "^*/+-";
    const std::string Numbers = "0123456789";
    std::string problem, answer;
    bool isSym(char) const;
    bool isNum(char) const;
    int open_parenthesis(std::string) const;
    int close_parenthesis(std::string) const;
    int findStartingCal(int, std::string) const;
    int findNumActions(std::string) const;
    int findNumNumbers(std::string) const;
    void initialize(std::string, double *, char *);
    int priorityAction(char *, int) const;
    int valueOfSym(char) const;
    double conversion(double &, double &, char) const;
    void vlueCHNGEarr(double *&, double, char *&, int, int);
    void problemEdit(std::string, int, int);
    std::string removePrecision(std::string);
public:
    cal();
    cal(std::string);
    ~cal();
    std::string getAns() const;
};

class color
{
private:
    std::string reset;
    int cSize;
    std::string *c = nullptr, *bc = nullptr;
public:
    color();
    ~color();
    std::string cReset() const;
    std::string getC(int) const;
    std::string getBC(int) const;
    int getSize() const;
};

class calMachine
{
private:
    const int usTime = 200, ulTime = 1000;
    std::string cursorRst; //ANSI escape code that sets terminal cursor back to (0,0)
public:
    calMachine();
    ~calMachine();
    void wScreen(std::string [], int);
    void calculator();
    void enterPScreen(int);
    bool existing(int [], int);
    bool existing(std::vector<int> &, int);
    int starting_effect(int, int, int);
    int findEffEnd(int, int);
    void remove_effect_element(int **, int, int &);
    void help_menu();
    bool is_cal_key(char);
    void print_to_screen(std::string &);
    void highlight_key(char);
};  


class BufferToggle
{
    private:
        struct termios t;

    public:

        /*
         * Disables buffered input
         */

        void off(void)
        {
            tcgetattr(STDIN_FILENO, &t); //get the current terminal I/O structure
            t.c_lflag &= ~ICANON; //Manipulate the flag bits to do what you want it to do
            tcsetattr(STDIN_FILENO, TCSANOW, &t); //Apply the new settings
        }


        /*
         * Enables buffered input
         */

        void on(void)
        {
            tcgetattr(STDIN_FILENO, &t); //get the current terminal I/O structure
            t.c_lflag |= ICANON; //Manipulate the flag bits to do what you want it to do
            tcsetattr(STDIN_FILENO, TCSANOW, &t); //Apply the new settings
        }
};
