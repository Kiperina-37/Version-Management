#include <stdio.h>
#include <assert.h> //Improve code robustness
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#define MAX_NAME_LEN 50
#define MAX_TEACHERS 100
#define PASSWORD 6
void Rolechoose();
void Welcome();
void Teacher();
void GetPass(char *pass);
typedef struct TEACHER
{
    char name[MAX_NAME_LEN];
    char password[PASSWORD];
    int Salary;
    int Coefficient;
    int Standard;
    int Time;
} TEACHER;
int TeacherLogin(struct TEACHER *teachers, int teacher_count);
int Teacherinf(struct TEACHER **teachers_ptr, int *teacher_count_ptr);
int main()
{
    Welcome();
    Rolechoose();
    return 0;
}
void Rolechoose()
{
    char choose[10];
    int valid_input = 0;

    while (!valid_input)
    {
        printf("Please input code(Teacher:1,Visitor:2)\n");
        if (fgets(choose, sizeof(choose), stdin) == NULL)
        {
            printf("Error reading input\n");
            continue;
        } // fgets is safer than grts
        size_t len = strlen(choose);
        if (len > 0 && choose[len - 1] == '\n')
        {
            choose[len - 1] = '\0';
        }
        else
        {
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
                ;
        }
        if (strlen(choose) == 1)
        {
            switch (choose[0])
            {
            case '1':
                printf("Teacher mode selected\n");
                Teacher();
                valid_input = 1;
                break;
            case '2':
                printf("Visitor mode selected\n");
                valid_input = 1;
                break;
            default:
                printf("What are you doing?\n");
            }
        }
        else
        {
            printf("Invalid input! Please enter only 1 or 2\n");
        }
    }
}
int Teacherinf(TEACHER **teachers_ptr, int *teacher_count_ptr)
{
    TEACHER *teachers = NULL;
    int count = 0;
    int box = 11;
    teachers = (TEACHER *)malloc(box * sizeof(TEACHER));
    if (teachers == NULL)
    {
        printf("Memory allocation failed\n");
        return 0;
    }
    FILE *file = fopen("inf.txt", "r+");
    if (file == NULL)
    {
        printf("Cannot open file\n");
        free(teachers);
        return 0;
    }
    printf("Reading file.. .\n");
    while (!feof(file))
    {
        if (count >= box)
        {
            box *= 2;
            TEACHER *temp = (TEACHER *)realloc(teachers, box * sizeof(TEACHER));
            if (temp == NULL)
            {
                printf("Memory reallocation failed\n");
                free(teachers);
                fclose(file);
                return 0;
            }
            teachers = temp;
        }
        TEACHER t;
        if (fscanf(file, "%s %s %d %d %d %d",
                   t.name,
                   t.password,
                   &t.Salary,
                   &t.Coefficient,
                   &t.Standard,
                   &t.Time) == 5)
        {
            teachers[count] = t;
            count++;
        }
        else
        {
            // Skip possible error lines
            char buffer[100];
            if (fgets(buffer, sizeof(buffer), file) == NULL)
                break;
        }
    }
    fclose(file);
    *teachers_ptr = teachers;
    *teacher_count_ptr = count;
    return 1;
}
void Teacher()
{
    system("cls");
    Welcome();
    TEACHER *teachers = NULL;
    int teacher_count = 0;
    if (!Teacherinf(&teachers, &teacher_count))
    {
        printf("Failed to load teacher information\n");
        return;
    }
    printf("Total teachers loaded: %d\n", teacher_count);
    int max_attempts = 3;
    int success = 0;
    for (int attempt = 1; attempt <= max_attempts && !success; attempt++)
    {
        printf("\nAttempt %d/%d\n", attempt, max_attempts);
        success = TeacherLogin(teachers, teacher_count);
        if (!success && attempt < max_attempts)
        {
            printf("Please try again\n");
        }
    }
    if (teachers != NULL)
    {
        free(teachers);
    }
    if (success)
    {
        printf("\nAccess granted to teacher functions\n");
        // API
    }
    else
    {
        printf("\nToo many failed attempts.Access denied\n");
    }
}
int TeacherLogin(TEACHER *teachers, int teacher_count)
{
    char username[MAX_NAME_LEN];
    char password[PASSWORD];
    printf("\n=== Teacher Login ===\n");
    printf("Username: ");
    scanf("%s", username);
    while (getchar() != '\n')
        ; // Skip possible error lines
    printf("Password: ");
    GetPass(password);
    for (int i = 0; i < teacher_count; i++)
    {
        if (strcmp(teachers[i].name, username) == 0 &&
            strcmp(password, teachers[i].password) == 0)
        {
            printf("\nLogin successful! Welcome %s\n", teachers[i].name);
            printf("Your information:\n");
            printf("  Salary: %d\n", teachers[i].Salary);
            printf("  Coefficient: %d\n", teachers[i].Coefficient);
            printf("  Standard: %d\n", teachers[i].Standard);
            printf("  Time: %d\n", teachers[i].Time);
            return 1;
        }
    }
    printf("\nLogin failed! Invalid username or password.\n");
    return 0;
}
void GetPass(char *pass)
{
    int i = 0;
    char ch;
    while (1)
    {
        ch = getch();
        if (ch == 13)
        {
            pass[i] = '\0';
            printf("\n");
            break;
        }
        else if (ch == 8)
        {
            if (i > 0)
            {
                i--;
                printf("\b \b");
            }
        }
        else if (i < MAX_NAME_LEN - 1)
        {
            pass[i] = ch;
            i++;
            printf("*");
        }
    }
}
void Welcome()
{
    printf("\n");
    printf("\t*********************************************************\n");
    printf("\n");
    printf("\t*                      Welcome User                     *\n");
    printf("\n");
    printf("\t*       Teacher Class Hour Fee Management System        *\n");
    printf("\n");
    printf("\t*********************************************************\n");
    printf("\n");
}