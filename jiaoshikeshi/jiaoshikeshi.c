#include <stdio.h>
#include <assert.h> //Improve code robustness
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <direct.h>
#include <windows.h>
#define MAX_NAME_LEN 50
#define MAX_TEACHERS 100
#define PASSWORD 8
#define VISITOR {"user"}
#define VISITOR_PASSWORD 123456
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
int TeacherLogin(TEACHER *teachers, int teacher_count, int *login_index);
int Teacherinf(struct TEACHER **teachers_ptr, int *teacher_count_ptr);
void Visitor();
void Menu();
void Choose_Menu(TEACHER *teachers, int teacher_count, int login_index, int from_teacher);
void Print_all_teachers(TEACHER *teachers, int teacher_count);
void ChangePassword(TEACHER *teacher, TEACHER *teachers, int *teacher_count);
void SaveTeachersToFile(TEACHER *teachers, int teacher_count);
void Sort_Salary(TEACHER *teachers, int teacher_count);
void Sort_Fee(TEACHER *teachers, int teacher_count);
void AddTeacher(TEACHER **teachers_ptr, int *teacher_count_ptr);
void DeleteTeacher(TEACHER **teachers_ptr, int *teacher_count_ptr, int current_login_index);
int main()
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    char cwd[1024];
    _getcwd(cwd, sizeof(cwd));
    printf("Current working directory: %s\n", cwd);
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
                Visitor();
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
                   &t.Time) == 6)
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
    int login_index = -1;
    for (int attempt = 1; attempt <= max_attempts && !success; attempt++)
    {
        printf("\nAttempt %d/%d\n", attempt, max_attempts);
        success = TeacherLogin(teachers, teacher_count, &login_index);
        if (!success && attempt < max_attempts)
        {
            printf("Please try again\n");
        }
    }
    if (success && login_index >= 0)
    {
        int class_hour_fee = (teachers[login_index].Time - teachers[login_index].Standard) *
                             teachers[login_index].Coefficient *
                             teachers[login_index].Salary;

        printf("\nAccess granted to teacher functions\n");
        printf("Calculating class hour fee...\n");
        printf("Time: %d, Standard: %d\n", teachers[login_index].Time, teachers[login_index].Standard);
        printf("Coefficient: %d, Salary: %d\n", teachers[login_index].Coefficient, teachers[login_index].Salary);
        printf("Class hour fee = (Time - Standard) * Coefficient * Salary\n");
        printf("Class hour fee = (%d - %d) * %d * %d\n",
               teachers[login_index].Time,
               teachers[login_index].Standard,
               teachers[login_index].Coefficient,
               teachers[login_index].Salary);
        printf("Class hour fee = %d\n", class_hour_fee);
        printf("\nPress enter to return to main menu...");
        getchar();
        Choose_Menu(teachers, teacher_count, login_index, 1);
    }
    else if (!success)
    {
        printf("\nToo many failed attempts. Access denied\n");
        printf("Press enter to return to main menu...");
        getchar();
        Choose_Menu(teachers, teacher_count, -1, 0);
    }

    if (teachers != NULL)
    {
        free(teachers);
    }
}
int TeacherLogin(TEACHER *teachers, int teacher_count, int *login_index)
{
    char username[MAX_NAME_LEN];
    char password[PASSWORD];
    printf("\n=== Teacher Login ===\n");
    printf("Username: ");
    fgets(username, sizeof(username), stdin);
    int len = strlen(username);
    if (username[len - 1] == '\n')
    {
        username[len - 1] = '\0';
    }
    printf("Password: ");
    GetPass(password);

    for (int i = 0; i < teacher_count; i++)
    {
        if (strcmp(teachers[i].name, username) == 0 &&
            strcmp(password, teachers[i].password) == 0)
        {
            printf("\nLogin successful! Welcome %s\n", teachers[i].name);
            printf("Your information:\n");
            printf("  Name: %s\n", teachers[i].name);
            printf("  Salary: %d\n", teachers[i].Salary);
            printf("  Coefficient: %d\n", teachers[i].Coefficient);
            printf("  Standard: %d\n", teachers[i].Standard);
            printf("  Time: %d\n", teachers[i].Time);
            *login_index = i;
            return 1;
        }
    }
    printf("\nLogin failed! Invalid username or password\n");
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
void Visitor()
{
    char User_name[MAX_NAME_LEN];
    char Input_name[] = VISITOR;
    int User_password;
    printf("\n=== Visitor Login ===\n");
    printf("Username: ");
    fgets(User_name, sizeof(User_name), stdin);
    int len = strlen(User_name);
    if (User_name[len - 1] == '\n')
    {
        User_name[len - 1] = '\0';
    }
    printf("User_password: ");
    scanf("%d", &User_password);
    while (getchar() != '\n')
        ;
    if ((User_password == VISITOR_PASSWORD) && (strcmp(User_name, Input_name) == 0))
    {
        system("cls");
        Welcome();
        Choose_Menu(NULL, 0, -1, 0);
        // API
    }
    else
    {
        printf("ERROR:Invalid username or password\n");
        printf("Press enter to return to main menu...");
        getchar();
        // API
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
void Menu()
{
    printf("\n");
    printf("\t*********************************************************\n");
    printf("\n");
    printf("\t*                  Please input code                    *\n");
    printf("\n");
    printf("\t*                  1:Addition                           *\n");
    printf("\t*                  2:Delete                             *\n");
    printf("\t*                  3:Fee Sort                           *\n");
    printf("\t*                  4:Salary Sort                        *\n");
    printf("\t*                  5:View Method Choose                 *\n");
    printf("\t*                  6:Save                               *\n");
    printf("\t*                  7:Return Visitor mode                *\n");
    printf("\t*                  8:Return Teacher mode                *\n");
    printf("\t*                  9:Change Password                    *\n");
    printf("\t*                  10:Print All Teachers                *\n");
    printf("\t*                  11:Close                             *\n");
    printf("\n");
    printf("\t*********************************************************\n");
    printf("\n");
}
void Choose_Menu(TEACHER *teachers, int teacher_count, int login_index, int from_teacher)
{
    // char choose[10];
    int choice = 0;
    while (1)
    {
        Menu();
        char choose[10];
        if (fgets(choose, sizeof(choose), stdin) == NULL)
        {
            printf("Error reading input\n");
            continue;
        } // fgets is safer than grts
        int len = strlen(choose);
        if (len > 0 && choose[len - 1] == '\n')
        {
            choose[len - 1] = '\0';
            len--;
        }
        else
        {
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
                ;
        }
        if (len == 0)
        {
            printf("Please enter a code\n");
        }
        int is_valid = 1;
        for (int i = 0; i < len; i++)
        {
            if (!isdigit(choose[i]))
            {
                is_valid = 0;
                break;
            }
        }
        if (!is_valid)
        {
            printf("Invalid input! Please enter a number between 1 and 11\n");
            continue;
        }
        choice = atoi(choose);
        switch (choice)
        {
        case 1:
            printf("Addetion mode selected\n");
            if (login_index >= 0 && teachers != NULL)
            {
                AddTeacher(&teachers, &teacher_count);
            }
            else
            {
                printf("Error: You must be logged in as a teacher to add new teachers\n");
                printf("Press enter to continue...");
                getchar();
            }
            // API
            break;
        case 2:
            printf("Delete mode selected\n");
            if (login_index >= 0 && teachers != NULL)
            {
                DeleteTeacher(&teachers, &teacher_count, login_index);
            }
            else
            {
                printf("Error: You must be logged in as a teacher to delete teachers\n");
                printf("Press enter to continue...");
                getchar();
            }
            // API
            break;
        case 3:
            printf("Fee Sort mode selected\n");
            if (teachers != NULL && teacher_count > 0)
            {
                TEACHER *sorted_teachers = (TEACHER *)malloc(teacher_count * sizeof(TEACHER));
                if (sorted_teachers == NULL)
                {
                    printf("Memory allocation failed\n");
                    break;
                }
                memcpy(sorted_teachers, teachers, teacher_count * sizeof(TEACHER));
                Sort_Fee(sorted_teachers, teacher_count);
                free(sorted_teachers);
            }
            else if (login_index >= 0 && teachers != NULL)
            {
                Sort_Fee(teachers, teacher_count);
            }
            else
            {
                TEACHER *temp_teachers = NULL;
                int temp_count = 0;
                if (Teacherinf(&temp_teachers, &temp_count))
                {
                    Sort_Fee(temp_teachers, temp_count);
                    if (temp_teachers != NULL)
                    {
                        free(temp_teachers);
                    }
                }
                else
                {
                    printf("Can't read file for sorting\n");
                }
            }
            // API
            break;
        case 4:
            printf("Salary Sort mode selected\n");
            if (teachers != NULL && teacher_count > 0)
            {
                TEACHER *sorted_teachers = (TEACHER *)malloc(teacher_count * sizeof(TEACHER));
                if (sorted_teachers == NULL)
                {
                    printf("Memory allocation failed\n");
                    break;
                }
                memcpy(sorted_teachers, teachers, teacher_count * sizeof(TEACHER));
                Sort_Salary(sorted_teachers, teacher_count);
                free(sorted_teachers);
            }
            else if (login_index >= 0 && teachers != NULL)
            {
                Sort_Salary(teachers, teacher_count);
            }
            else
            {
                TEACHER *temp_teachers = NULL;
                int temp_count = 0;
                if (Teacherinf(&temp_teachers, &temp_count))
                {
                    Sort_Salary(temp_teachers, temp_count);
                    if (temp_teachers != NULL)
                    {
                        free(temp_teachers);
                    }
                }
                else
                {
                    printf("Can't read file for sorting\n");
                }
            }
            break;
        case 5:
            printf("View Method mode selected\n");
            printf("\n=== Current Mode Information ===\n");
            if (login_index >= 0 && teachers != NULL)
            {
                printf("Current Mode: Teacher Mode\n");
                printf("Logged in as: %s\n", teachers[login_index].name);
                printf("Login Index: %d\n", login_index + 1);
                printf("Teacher Information:\n");
                printf("  Name: %s\n", teachers[login_index].name);
                printf("  Salary: %d\n", teachers[login_index].Salary);
                printf("  Coefficient: %d\n", teachers[login_index].Coefficient);
                printf("  Standard: %d\n", teachers[login_index].Standard);
                printf("  Time: %d\n", teachers[login_index].Time);
                int overtime = teachers[login_index].Time - teachers[login_index].Standard;
                int class_hour_fee = overtime * teachers[login_index].Coefficient * teachers[login_index].Salary;
                printf("  Overtime Hours: %d\n", overtime);
                printf("  Class Hour Fee: %d\n", class_hour_fee);
                printf("Permissions:\n");
                printf("  ✓ Add Data\n");
                printf("  ✓ Delete Data");
                printf("  ✓ Change Password\n");
                printf("  ✓ Save Data\n");
                printf("  ✓ Switch to Teacher Mode (requires login)\n");
                printf("  ✓ Access Teacher Functions\n");
            }
            else
            {
                printf("Current Mode: Visitor Mode\n");
                printf("Permissions:\n");
                printf("  ✓ View All Teachers Information\n");
                printf("  ✓ Sort All Teachers Salary And Fee\n");
                printf("  ✓ Switch to Teacher Mode (requires login)\n");
                printf("  ✗ Change Password (requires teacher login)\n");
                printf("  ✗ Modify Data (requires teacher login)\n");
                printf("  ✗ Add Data (requires teacher login)\n");
                printf("  ✗ Delete Data (requires teacher login)\n");
            }
            printf("\nSystem Status:\n");
            printf("  Total Teachers in System: %d\n", teacher_count);
            printf("  Current Login Status: %s\n", login_index >= 0 ? "Logged In" : "Not Logged In");
            if (from_teacher)
            {
                printf("Entered From: Teacher Mode\n");
            }
            else
            {
                printf("Entered From: Visitor Mode\n");
            }
            printf("\n=== End of Mode Information ===\n");
            // API
            break;
        case 6:
            printf("Save mode selected\n");
            if (teachers != NULL && teacher_count > 0)
            {
                SaveTeachersToFile(teachers, teacher_count);
            }
            else
            {
                printf("No teacher information to save\n");
            }
            // API
            break;
        case 7:
            printf("Return Visitor mode selected\n");
            if (from_teacher)
            {
                if (teachers != NULL)
                {
                    free(teachers);
                }
                system("cls");
                Welcome();
                Visitor();
                return;
            }
            else
            {
                Visitor();
                return;
            }
            // API
            break;
        case 8:
            printf("Return Teacher mode selected\n");
            if (!from_teacher)
            {
                system("cls");
                Welcome();
                Teacher();
                return;
            }
            else
            {
                return;
            }
            // API
            break;
        case 9:
            printf("Change Password mode selected\n");
            if (login_index >= 0 && teachers != NULL)
            {
                ChangePassword(&teachers[login_index], teachers, &teacher_count);
            }
            else
            {
                printf("Error: You must be logged in as a teacher to change password!\n");
                printf("Press enter to continue...");
                getchar();
            }
            // API
            break;
        case 10:
            printf("Print all teachers mode selected\n");
            TEACHER *temp_teachers = NULL;
            int temp_count = 0;
            if (Teacherinf(&temp_teachers, &temp_count))
            {
                Print_all_teachers(temp_teachers, temp_count);
                if (temp_teachers != NULL)
                {
                    free(temp_teachers);
                }
                else
                {
                    printf("Can't read file");
                }
            }
            break;
        case 11:
            printf("Close mode selected\n");
            exit(0);
        default:
            printf("Invalid choice! Please enter a number between 1 and 11\n");
        }
        printf("\nPress enter to continue...");
        while (getchar() != '\n')
            ;
    }
}
void Print_all_teachers(TEACHER *teachers, int teacher_count)
{
    if (teachers == NULL || teacher_count == 0)
    {
        printf("No information");
        return;
    }
    printf("\n");
    printf("========================================================================================================\n");
    printf("                                        All data                                                        \n");
    printf("========================================================================================================\n");
    printf("No. %-12s %-10s %-8s %-12s %-12s %-12s %-15s\n",
           "Name", "Salary", "Coefficient", "Standard", "Time", "Overtime", "Fee");
    printf("--------------------------------------------------------------------------------------------------------\n");
    int total_fee = 0;
    for (int i = 0; i < teacher_count; i++)
    {
        int overtime = teachers[i].Time - teachers[i].Standard;
        int fee = overtime * teachers[i].Coefficient * teachers[i].Salary;
        total_fee += fee;
        printf("%-15s %-10d %-10d %-15d %-15d %-10d %-15d\n",
               teachers[i].name,
               teachers[i].Salary,
               teachers[i].Coefficient,
               teachers[i].Standard,
               teachers[i].Time,
               overtime,
               fee);
    }
    printf("================================================================================\n");
    printf("total %d teachers,totally fee:%d\n", teacher_count, total_fee);
    printf("================================================================================\n");
    printf("\n");
}
void SaveTeachersToFile(TEACHER *teachers, int teacher_count)
{
    FILE *file = fopen("inf.txt", "w");
    if (file == NULL)
    {
        printf("Cannot open file for writing\n");
        return;
    }
    for (int i = 0; i < teacher_count; i++)
    {
        fprintf(file, "%s %s %d %d %d %d\n",
                teachers[i].name,
                teachers[i].password,
                teachers[i].Salary,
                teachers[i].Coefficient,
                teachers[i].Standard,
                teachers[i].Time);
    }
    fclose(file);
    printf("Teacher information saved successfully\n");
}
void ChangePassword(TEACHER *teacher, TEACHER *teachers, int *teacher_count)
{
    char old_password[PASSWORD];
    char new_password[PASSWORD];
    char confirm_password[PASSWORD];
    printf("\n=== Change Password ===\n");
    printf("Current user: %s\n", teacher->name);
    printf("Enter old password: ");
    GetPass(old_password);
    if (strcmp(old_password, teacher->password) != 0)
    {
        printf("Error: Old password is incorrect\n");
        printf("Press enter to continue...");
        getchar();
        return;
    }
    printf("Enter new password: ");
    GetPass(new_password);
    if (strlen(new_password) == 0)
    {
        printf("Error: New password cannot be empty\n");
        printf("Press enter to continue...");
        getchar();
        return;
    }
    printf("Confirm new password: ");
    GetPass(confirm_password);
    if (strcmp(new_password, confirm_password) != 0)
    {
        printf("Error: New passwords do not match\n");
        printf("Press enter to continue...");
        getchar();
        return;
    }
    if (strcmp(new_password, teacher->password) == 0)
    {
        printf("Error: New password must be different from old password\n");
        printf("Press enter to continue...");
        getchar();
        return;
    }
    strcpy(teacher->password, new_password);
    for (int i = 0; i < *teacher_count; i++)
    {
        if (strcmp(teachers[i].name, teacher->name) == 0)
        {
            strcpy(teachers[i].password, new_password);
            break;
        }
    }
    SaveTeachersToFile(teachers, *teacher_count);
    printf("Password changed successfully\n");
    printf("Press enter to continue...");
    getchar();
}
void Sort_Salary(TEACHER *teachers, int teacher_count)
{
    if (teachers == NULL || teacher_count == 0)
    {
        printf("No teacher information to sort\n");
        return;
    }
    printf("\n=== Salary Sort ===\n");
    printf("Sorting teachers by salary...\n");
    for (int i = 0; i < teacher_count - 1; i++)
    {
        for (int j = 0; j < teacher_count - i - 1; j++)
        {
            if (teachers[j].Salary > teachers[j + 1].Salary)
            {
                TEACHER temp = teachers[j];
                teachers[j] = teachers[j + 1];
                teachers[j + 1] = temp;
            }
        }
    }
    printf("Sort completed! Displaying sorted results:\n\n");
    printf("========================================================================================================\n");
    printf("                           Teachers Sorted by Salary (Ascending Order)                                  \n");
    printf("========================================================================================================\n");
    printf("No. %-12s %-10s %-8s %-12s %-12s %-12s %-15s\n",
           "Name", "Salary", "Coefficient", "Standard", "Time", "Overtime", "Fee");
    printf("--------------------------------------------------------------------------------------------------------\n");
    int total_fee = 0;
    for (int i = 0; i < teacher_count; i++)
    {
        int overtime = teachers[i].Time - teachers[i].Standard;
        int fee = overtime * teachers[i].Coefficient * teachers[i].Salary;
        total_fee += fee;
        printf("%-4d %-12s %-10d %-8d %-12d %-12d %-12d %-15d\n",
               i + 1,
               teachers[i].name,
               teachers[i].Salary,
               teachers[i].Coefficient,
               teachers[i].Standard,
               teachers[i].Time,
               overtime,
               fee);
    }
    printf("========================================================================================================\n");
    printf("Sorted %d teachers by salary (ascending order). Total fee: %d\n", teacher_count, total_fee);
    printf("========================================================================================================\n");
    printf("\nDo you want to save the sorted results? (y/n): ");
    char choice;
    scanf(" %c", &choice);
    while (getchar() != '\n')
        ;
    if (choice == 'y' || choice == 'Y')
    {
        SaveTeachersToFile(teachers, teacher_count);
        printf("Sorted results have been saved to file\n");
    }
    else
    {
        printf("Sorted results not saved\n");
    }
    printf("\nPress enter to continue...");
}
void Sort_Fee(TEACHER *teachers, int teacher_count)
{
    if (teachers == NULL || teacher_count == 0)
    {
        printf("No teacher information to sort\n");
        return;
    }
    printf("\n=== Fee Sort ===\n");
    printf("Sorting teachers by class hour fee...\n");
    for (int i = 0; i < teacher_count - 1; i++)
    {
        for (int j = 0; j < teacher_count - i - 1; j++)
        {
            int fee_j = (teachers[j].Time - teachers[j].Standard) *
                        teachers[j].Coefficient * teachers[j].Salary;
            int fee_j1 = (teachers[j + 1].Time - teachers[j + 1].Standard) *
                         teachers[j + 1].Coefficient * teachers[j + 1].Salary;
            if (fee_j < fee_j1)
            {
                TEACHER temp = teachers[j];
                teachers[j] = teachers[j + 1];
                teachers[j + 1] = temp;
            }
        }
    }

    printf("Sort completed! Displaying sorted results (from highest to lowest):\n\n");
    int total_fee = 0;
    int max_fee = 0;
    int min_fee = 0;
    if (teacher_count > 0)
    {
        max_fee = (teachers[0].Time - teachers[0].Standard) *
                  teachers[0].Coefficient * teachers[0].Salary;
        min_fee = (teachers[teacher_count - 1].Time - teachers[teacher_count - 1].Standard) *
                  teachers[teacher_count - 1].Coefficient * teachers[teacher_count - 1].Salary;
    }

    printf("========================================================================================================\n");
    printf("                    Teachers Sorted by Class Hour Fee (Descending Order - Highest to Lowest)            \n");
    printf("========================================================================================================\n");
    printf("No. %-12s %-10s %-8s %-12s %-12s %-12s %-15s\n",
           "Name", "Salary", "Coefficient", "Standard", "Time", "Overtime", "Fee");
    printf("--------------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < teacher_count; i++)
    {
        int overtime = teachers[i].Time - teachers[i].Standard;
        int fee = overtime * teachers[i].Coefficient * teachers[i].Salary;
        total_fee += fee;
        if (i < 3)
        {
            char rank_mark[10];
            if (i == 0)
                strcpy(rank_mark, "1");
            else if (i == 1)
                strcpy(rank_mark, "2");
            else
                strcpy(rank_mark, "3");

            printf("%-4d%-2s %-12s %-10d %-8d %-12d %-12d %-12d %-15d\n",
                   i + 1, rank_mark,
                   teachers[i].name,
                   teachers[i].Salary,
                   teachers[i].Coefficient,
                   teachers[i].Standard,
                   teachers[i].Time,
                   overtime,
                   fee);
        }
        else
        {
            printf("%-4d %-12s %-10d %-8d %-12d %-12d %-12d %-15d\n",
                   i + 1,
                   teachers[i].name,
                   teachers[i].Salary,
                   teachers[i].Coefficient,
                   teachers[i].Standard,
                   teachers[i].Time,
                   overtime,
                   fee);
        }
    }
    printf("========================================================================================================\n");
    printf("Fee Statistics:\n");
    printf("  Highest Fee: %d (Teacher: %s)\n", max_fee, teacher_count > 0 ? teachers[0].name : "N/A");
    printf("  Lowest Fee: %d (Teacher: %s)\n", min_fee, teacher_count > 0 ? teachers[teacher_count - 1].name : "N/A");
    printf("  Average Fee: %.2f\n", teacher_count > 0 ? (float)total_fee / teacher_count : 0);
    printf("  Total Fee: %d\n", total_fee);
    printf("========================================================================================================\n");
    printf("\n=== Fee Analysis ===\n");
    printf("1. Top 3 Teachers with Highest Fees\n");
    for (int i = 0; i < teacher_count && i < 3; i++)
    {
        int fee = (teachers[i].Time - teachers[i].Standard) *
                  teachers[i].Coefficient * teachers[i].Salary;
        printf("   %d. %s: %d (Salary: %d, Coefficient: %d, Overtime: %d)\n",
               i + 1, teachers[i].name, fee,
               teachers[i].Salary, teachers[i].Coefficient,
               teachers[i].Time - teachers[i].Standard);
    }
    if (teacher_count > 3)
    {
        printf("\n2. Teachers with Fees Below Average\n");
        float avg_fee = (float)total_fee / teacher_count;
        int below_avg_count = 0;
        for (int i = 0; i < teacher_count; i++)
        {
            int fee = (teachers[i].Time - teachers[i].Standard) *
                      teachers[i].Coefficient * teachers[i].Salary;
            if (fee < avg_fee)
            {
                printf("   - %s: %d (%.2f%% below average)\n",
                       teachers[i].name, fee,
                       ((avg_fee - fee) / avg_fee) * 100);
                below_avg_count++;
            }
        }
        printf("   Total: %d teachers (%.1f%%)\n", below_avg_count,
               (float)below_avg_count / teacher_count * 100);
    }
    printf("\nDo you want to save the sorted results? (y/n): ");
    char choice;
    scanf(" %c", &choice);
    while (getchar() != '\n')
        ;
    if (choice == 'y' || choice == 'Y')
    {
        SaveTeachersToFile(teachers, teacher_count);
        printf("Sorted results have been saved to file\n");
    }
    else
    {
        printf("Sorted results not saved.\n");
    }
    printf("\nPress enter to continue...");
}
void AddTeacher(TEACHER **teachers_ptr, int *teacher_count_ptr)
{
    if (*teachers_ptr == NULL)
    {
        printf("Error: Teacher information not loaded\n");
        printf("Press enter to continue...");
        getchar();
        return;
    }
    printf("\n=== Add New Teacher ===\n");
    TEACHER new_teacher;
    printf("Enter teacher name: ");
    fgets(new_teacher.name, sizeof(new_teacher.name), stdin);
    int len = strlen(new_teacher.name);
    if (new_teacher.name[len - 1] == '\n')
    {
        new_teacher.name[len - 1] = '\0';
    }
    for (int i = 0; i < *teacher_count_ptr; i++)
    {
        if (strcmp((*teachers_ptr)[i].name, new_teacher.name) == 0)
        {
            printf("Error: Teacher with name '%s' already exists\n", new_teacher.name);
            printf("Press enter to continue...");
            getchar();
            return;
        }
    }
    char password1[PASSWORD];
    char password2[PASSWORD];
    printf("Enter password: ");
    GetPass(password1);
    printf("Confirm password: ");
    GetPass(password2);
    if (strcmp(password1, password2) != 0)
    {
        printf("Error: Passwords do not match\n");
        printf("Press enter to continue...");
        getchar();
        return;
    }
    if (strlen(password1) < 4)
    {
        printf("Error: Password must be at least 4 characters long\n");
        printf("Press enter to continue...");
        getchar();
        return;
    }
    strcpy(new_teacher.password, password1);
    printf("Enter salary: ");
    char salary_input[20];
    fgets(salary_input, sizeof(salary_input), stdin);
    new_teacher.Salary = atoi(salary_input);
    if (new_teacher.Salary <= 0)
    {
        printf("Error: Salary must be a positive numbe\n");
        printf("Press enter to continue...");
        getchar();
        return;
    }
    printf("Enter coefficient: ");
    char coeff_input[20];
    fgets(coeff_input, sizeof(coeff_input), stdin);
    new_teacher.Coefficient = atoi(coeff_input);
    if (new_teacher.Coefficient <= 0)
    {
        printf("Error: Coefficient must be a positive number\n");
        printf("Press enter to continue...");
        getchar();
        return;
    }
    printf("Enter standard hours: ");
    char std_input[20];
    fgets(std_input, sizeof(std_input), stdin);
    new_teacher.Standard = atoi(std_input);
    if (new_teacher.Standard < 0)
    {
        printf("Error: Standard hours cannot be negative\n");
        printf("Press enter to continue...");
        getchar();
        return;
    }
    printf("Enter actual hours: ");
    char time_input[20];
    fgets(time_input, sizeof(time_input), stdin);
    new_teacher.Time = atoi(time_input);
    if (new_teacher.Time < 0)
    {
        printf("Error: Actual hours cannot be negative\n");
        printf("Press enter to continue...");
        getchar();
        return;
    }
    if (new_teacher.Time < new_teacher.Standard)
    {
        printf("Warning: Actual hours (%d) are less than standard hours (%d)\n",
               new_teacher.Time, new_teacher.Standard);
        printf("The teacher may not receive overtime pay\n");
        printf("Do you want to continue? (y/n): ");
        char confirm;
        scanf(" %c", &confirm);
        while (getchar() != '\n')
            ;
        if (confirm != 'y' && confirm != 'Y')
        {
            printf("Teacher addition cancelled\n");
            printf("Press enter to continue...");
            getchar();
            return;
        }
    }
    int new_count = *teacher_count_ptr + 1;
    TEACHER *new_teachers = (TEACHER *)realloc(*teachers_ptr, new_count * sizeof(TEACHER));
    if (new_teachers == NULL)
    {
        printf("Error: Memory allocation failed\n");
        printf("Press enter to continue...");
        getchar();
        return;
    }
    new_teachers[*teacher_count_ptr] = new_teacher;
    *teachers_ptr = new_teachers;
    *teacher_count_ptr = new_count;
    SaveTeachersToFile(*teachers_ptr, *teacher_count_ptr);
    int overtime = new_teacher.Time - new_teacher.Standard;
    int class_hour_fee = overtime > 0 ? overtime * new_teacher.Coefficient * new_teacher.Salary : 0;
    printf("\n=== Teacher Added Successfully ===\n");
    printf("Name: %s\n", new_teacher.name);
    printf("Salary: %d\n", new_teacher.Salary);
    printf("Coefficient: %d\n", new_teacher.Coefficient);
    printf("Standard Hours: %d\n", new_teacher.Standard);
    printf("Actual Hours: %d\n", new_teacher.Time);
    printf("Overtime Hours: %d\n", overtime);
    printf("Class Hour Fee: %d\n", class_hour_fee);
    printf("===================================\n");
    printf("\nPress enter to continue...");
    getchar();
}
void DeleteTeacher(TEACHER **teachers_ptr, int *teacher_count_ptr, int current_login_index)
{
    if (*teachers_ptr == NULL || *teacher_count_ptr == 0)
    {
        printf("Error: No teacher information available\n");
        printf("Press enter to continue...");
        getchar();
        return;
    }
    printf("\n=== Delete Teacher ===\n");
    printf("Current Teachers:\n");
    printf("============================================\n");
    for (int i = 0; i < *teacher_count_ptr; i++)
    {
        printf("%d. %s", i + 1, (*teachers_ptr)[i].name);
        if (i == current_login_index)
        {
            printf(" (Current User)");
        }
        printf("\n");
    }
    printf("============================================\n");
    char teacher_name[MAX_NAME_LEN];
    printf("\nEnter the name of teacher to delete: ");
    fgets(teacher_name, sizeof(teacher_name), stdin);
    int len = strlen(teacher_name);
    if (teacher_name[len - 1] == '\n')
    {
        teacher_name[len - 1] = '\0';
    }
    if (strlen(teacher_name) == 0)
    {
        printf("Error: Teacher name cannot be empty\n");
        printf("Press enter to continue...");
        getchar();
        return;
    }
    if (current_login_index >= 0 &&
        strcmp((*teachers_ptr)[current_login_index].name, teacher_name) == 0)
    {
        printf("Error: You cannot delete your own account while logged in\n");
        printf("Please ask another teacher or administrator to delete your account\n");
        printf("Press enter to continue...");
        getchar();
        return;
    }
    int delete_index = -1;
    for (int i = 0; i < *teacher_count_ptr; i++)
    {
        if (strcmp((*teachers_ptr)[i].name, teacher_name) == 0)
        {
            delete_index = i;
            break;
        }
    }
    if (delete_index == -1)
    {
        printf("Error: Teacher '%s' not found\n", teacher_name);
        printf("Press enter to continue...");
        getchar();
        return;
    }
    printf("\n=== Teacher Information to Delete ===\n");
    TEACHER *teacher_to_delete = &(*teachers_ptr)[delete_index];
    printf("Name: %s\n", teacher_to_delete->name);
    printf("Salary: %d\n", teacher_to_delete->Salary);
    printf("Coefficient: %d\n", teacher_to_delete->Coefficient);
    printf("Standard Hours: %d\n", teacher_to_delete->Standard);
    printf("Actual Hours: %d\n", teacher_to_delete->Time);
    int overtime = teacher_to_delete->Time - teacher_to_delete->Standard;
    int class_hour_fee = overtime > 0 ? overtime * teacher_to_delete->Coefficient * teacher_to_delete->Salary : 0;
    printf("Overtime Hours: %d\n", overtime);
    printf("Class Hour Fee: %d\n", class_hour_fee);
    printf("=====================================\n");
    printf("\nAre you sure you want to delete teacher '%s'? (y/n): ", teacher_name);
    char confirm;
    scanf(" %c", &confirm);
    while (getchar() != '\n')
        ;
    if (confirm != 'y' && confirm != 'Y')
    {
        printf("Deletion cancelled\n");
        printf("Press enter to continue...");
        getchar();
        return;
    }
    if (*teacher_count_ptr == 1)
    {
        free(*teachers_ptr);
        *teachers_ptr = NULL;
        *teacher_count_ptr = 0;
    }
    else
    {
        TEACHER *new_teachers = (TEACHER *)malloc((*teacher_count_ptr - 1) * sizeof(TEACHER));
        if (new_teachers == NULL)
        {
            printf("Error: Memory allocation failed\n");
            printf("Press enter to continue...");
            getchar();
            return;
        }
        int new_index = 0;
        for (int i = 0; i < *teacher_count_ptr; i++)
        {
            if (i != delete_index)
            {
                new_teachers[new_index] = (*teachers_ptr)[i];
                new_index++;
            }
        }
        free(*teachers_ptr);
        *teachers_ptr = new_teachers;
        (*teacher_count_ptr)--;
    }
    SaveTeachersToFile(*teachers_ptr, *teacher_count_ptr);
    printf("\n=== Teacher Deleted Successfully ===\n");
    printf("Teacher '%s' has been removed from the system\n", teacher_name);
    printf("Remaining teachers: %d\n", *teacher_count_ptr);
    if (current_login_index >= 0 && delete_index < current_login_index)
    {
        current_login_index--;
        printf("\nNote: Your login index has been updated due to the deletion\n");
    }

    printf("\nPress enter to continue...");
    getchar();
}