#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <direct.h>
#include <windows.h>
#define MAX_NAME_LEN 50
#define MAX_TEACHERS 100
#define PASSWORD_LEN 32
#define VISITOR "user"
#define VISITOR_PASSWORD "123456"
#define INF_FILE "inf.txt"
#define BACKUP_FILE "inf_backup.txt"
typedef struct TEACHER
{
    char name[MAX_NAME_LEN];
    char password[PASSWORD_LEN];
    int Salary;
    int Coefficient;
    int Standard;
    int Time;
    int Fee;
} TEACHER;
void Rolechoose();
void Welcome();
void Teacher();
void GetPass(char *pass, int max_len);
int CalculateFee(TEACHER *teacher);
int LoadTeachersFromFile(const char *filename, TEACHER **teachers_ptr, int *teacher_count_ptr);
void SaveTeachersToFile(TEACHER *teachers, int teacher_count); // 修改：改为void类型，移除filename参数
void CreateBackup(const char *filename);
int QuickSortTeachers(TEACHER *teachers, int left, int right, int sort_by);
int CompareTeachers(const TEACHER *a, const TEACHER *b, int sort_by);
int TeacherLogin(TEACHER *teachers, int teacher_count, int *login_index);
int Teacherinf(struct TEACHER **teachers_ptr, int *teacher_count_ptr);
void Visitor();
void Menu();
void Choose_Menu(TEACHER *teachers, int teacher_count, int login_index, int from_teacher);
void Print_all_teachers(TEACHER *teachers, int teacher_count);
void ChangePassword(TEACHER *teacher, TEACHER *teachers, int *teacher_count);
void Sort_Salary(TEACHER *teachers, int teacher_count);
void Sort_Fee(TEACHER *teachers, int teacher_count);
void AddTeacher(TEACHER **teachers_ptr, int *teacher_count_ptr);
void DeleteTeacher(TEACHER **teachers_ptr, int *teacher_count_ptr, int current_login_index);
void BackupData();
void RestoreData();
void ExportToCSV(TEACHER *teachers, int teacher_count);
void SearchTeacher(TEACHER *teachers, int teacher_count);
void Statistics(TEACHER *teachers, int teacher_count);
void QuickSort(TEACHER *teachers, int left, int right, int sort_by);
int main()
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    BackupData();
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
        printf("请选择登录模式(教师:1, 访客:2, 退出:0)\n");
        if (fgets(choose, sizeof(choose), stdin) == NULL)
        {
            printf("输入读取错误\n");
            continue;
        }
        size_t len = strlen(choose);
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
        if (strlen(choose) == 1)
        {
            switch (choose[0])
            {
            case '1':
                printf("教师模式\n");
                Teacher();
                valid_input = 1;
                break;
            case '2':
                printf("访客模式\n");
                Visitor();
                valid_input = 1;
                break;
            case '0':
                printf("感谢使用，再见！\n");
                exit(0);
            default:
                printf("无效选择！请输入0, 1或2\n");
            }
        }
        else
        {
            printf("无效输入！请输入单个数字\n");
        }
    }
}
int Teacherinf(TEACHER **teachers_ptr, int *teacher_count_ptr)
{
    TEACHER *teachers = NULL;
    int count = 0;
    int capacity = 10;
    teachers = (TEACHER *)malloc(capacity * sizeof(TEACHER));
    if (teachers == NULL)
    {
        printf("内存分配失败\n");
        return 0;
    }
    FILE *file = fopen(INF_FILE, "r");
    if (file == NULL)
    {
        printf("无法打开文件，将创建新文件\n");
        file = fopen(INF_FILE, "w");
        if (file)
            fclose(file);
        *teachers_ptr = teachers;
        *teacher_count_ptr = 0;
        return 1;
    }
    printf("正在读取文件...\n");
    char line[256];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (line[0] == '\n' || line[0] == '#')
            continue;
        if (count >= capacity)
        {
            capacity *= 2;
            TEACHER *temp = (TEACHER *)realloc(teachers, capacity * sizeof(TEACHER));
            if (temp == NULL)
            {
                printf("内存重新分配失败\n");
                free(teachers);
                fclose(file);
                return 0;
            }
            teachers = temp;
        }
        TEACHER t;
        memset(&t, 0, sizeof(TEACHER));
        if (sscanf(line, "%49s %31s %d %d %d %d",
                   t.name,
                   t.password,
                   &t.Salary,
                   &t.Coefficient,
                   &t.Standard,
                   &t.Time) == 6)
        {
            t.Fee = CalculateFee(&t);
            if (t.Salary <= 0 || t.Coefficient <= 0 || t.Standard < 0 || t.Time < 0)
            {
                printf("警告：教师 %s 的数据异常，已跳过\n", t.name);
                continue;
            }
            teachers[count] = t;
            count++;
        }
        else
        {
            printf("警告：跳过格式错误的行: %s", line);
        }
    }
    fclose(file);
    if (count == 0 && teachers != NULL)
    {
        TEACHER *temp = (TEACHER *)realloc(teachers, sizeof(TEACHER));
        if (temp != NULL)
            teachers = temp;
    }
    *teachers_ptr = teachers;
    *teacher_count_ptr = count;
    printf("成功加载 %d 名教师信息\n", count);
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
        printf("教师信息加载失败\n");
        printf("按回车键返回主菜单...");
        getchar();
        Rolechoose();
        return;
    }
    printf("系统中共有 %d 名教师\n", teacher_count);
    int max_attempts = 3;
    int success = 0;
    int login_index = -1;
    for (int attempt = 1; attempt <= max_attempts && !success; attempt++)
    {
        printf("\n第 %d/%d 次尝试\n", attempt, max_attempts);
        success = TeacherLogin(teachers, teacher_count, &login_index);

        if (!success && attempt < max_attempts)
        {
            printf("登录失败，请重试\n");
            Sleep(1000);
        }
    }
    if (success && login_index >= 0)
    {
        int class_hour_fee = CalculateFee(&teachers[login_index]);
        printf("\n教师功能已解锁\n");
        printf("欢迎您，%s 老师！\n", teachers[login_index].name);
        printf("\n您的课时费计算详情：\n");
        printf("实际课时：%d，标准课时：%d\n", teachers[login_index].Time, teachers[login_index].Standard);
        printf("课时系数：%d，基本工资：%d\n", teachers[login_index].Coefficient, teachers[login_index].Salary);
        if (teachers[login_index].Time > teachers[login_index].Standard)
        {
            printf("超课时费 = (实际课时 - 标准课时) × 课时系数 × 基本工资\n");
            printf("超课时费 = (%d - %d) × %d × %d\n",
                   teachers[login_index].Time,
                   teachers[login_index].Standard,
                   teachers[login_index].Coefficient,
                   teachers[login_index].Salary);
        }
        printf("您的总课时费：%d元\n", class_hour_fee);
        printf("\n按回车键继续...");
        getchar();
        Choose_Menu(teachers, teacher_count, login_index, 1);
    }
    else
    {
        printf("\n登录失败次数过多，请稍后再试\n");
        printf("按回车键返回主菜单...");
        getchar();
        if (teachers != NULL)
        {
            free(teachers);
        }
        Rolechoose();
    }
}
int TeacherLogin(TEACHER *teachers, int teacher_count, int *login_index)
{
    char username[MAX_NAME_LEN];
    char password[PASSWORD_LEN];
    printf("\n=== 教师登录 ===\n");
    printf("用户名：");
    if (fgets(username, sizeof(username), stdin) == NULL)
    {
        return 0;
    }
    size_t len = strlen(username);
    if (len > 0 && username[len - 1] == '\n')
    {
        username[len - 1] = '\0';
    }
    printf("密码：");
    GetPass(password, PASSWORD_LEN);
    for (int i = 0; i < teacher_count; i++)
    {
        if (strcmp(teachers[i].name, username) == 0 &&
            strcmp(password, teachers[i].password) == 0)
        {
            printf("\n登录成功！欢迎 %s 老师\n", teachers[i].name);
            *login_index = i;
            return 1;
        }
    }
    printf("\n登录失败！用户名或密码错误\n");
    return 0;
}
void GetPass(char *pass, int max_len)
{
    int i = 0;
    char ch;
    while (i < max_len - 1)
    {
        ch = _getch();

        if (ch == '\r' || ch == '\n')
        {
            pass[i] = '\0';
            printf("\n");
            break;
        }
        else if (ch == '\b' || ch == 127)
        {
            if (i > 0)
            {
                i--;
                printf("\b \b");
            }
        }
        else if (isprint(ch))
        {
            pass[i] = ch;
            i++;
            printf("*");
        }
    }
    pass[i] = '\0';
}
void Visitor()
{
    system("cls");
    Welcome();
    char User_name[MAX_NAME_LEN];
    char User_password[PASSWORD_LEN];
    printf("\n=== 访客登录 ===\n");
    printf("用户名：");
    if (fgets(User_name, sizeof(User_name), stdin) == NULL)
    {
        printf("输入错误\n");
        return;
    }
    size_t len = strlen(User_name);
    if (len > 0 && User_name[len - 1] == '\n')
    {
        User_name[len - 1] = '\0';
    }
    printf("密码：");
    GetPass(User_password, PASSWORD_LEN);
    while (getchar() != '\n')
        ;
    if (strcmp(User_password, VISITOR_PASSWORD) == 0 &&
        strcmp(User_name, VISITOR) == 0)
    {
        printf("\n访客登录成功！\n");
        Choose_Menu(NULL, 0, -1, 0);
    }
    else
    {
        printf("\n错误：用户名或密码不正确\n");
        printf("按回车键返回主菜单...");
        getchar();
        Rolechoose();
    }
}
void Welcome()
{
    printf("\n");
    printf("\t*********************************************************\n");
    printf("\n");
    printf("\t*              教师课时费管理系统                       *\n");
    printf("\n");
    printf("\t*              欢迎使用本系统                           *\n");
    printf("\n");
    printf("\t*             版本：2.0                                *\n");
    printf("\n");
    printf("\t*********************************************************\n");
    printf("\n");
}
void Menu()
{
    printf("\n");
    printf("\t=========================================================\n");
    printf("\t                    功能菜单\n");
    printf("\t=========================================================\n");
    printf("\t  1. 添加教师          2. 删除教师\n");
    printf("\t  3. 课时费排序        4. 工资排序\n");
    printf("\t  5. 查看系统信息      6. 保存数据\n");
    printf("\t  7. 返回访客模式      8. 返回教师模式\n");
    printf("\t  9. 修改密码         10. 显示所有教师\n");
    printf("\t 11. 搜索教师         12. 数据统计\n");
    printf("\t 13. 导出数据         14. 备份/恢复\n");
    printf("\t  0. 退出系统\n");
    printf("\t=========================================================\n");
    printf("\n请选择操作 (0-14): ");
}
void Choose_Menu(TEACHER *teachers, int teacher_count, int login_index, int from_teacher)
{
    int choice = -1;
    char input[32];
    if (!from_teacher && teachers == NULL)
    {
        if (!Teacherinf(&teachers, &teacher_count))
        {
            printf("无法加载教师数据\n");
            teachers = NULL;
            teacher_count = 0;
        }
    }
    while (1)
    {
        system("cls");
        Welcome();
        if (from_teacher && login_index >= 0 && teachers != NULL)
        {
            printf("当前用户: %s [教师模式]\n", teachers[login_index].name);
            printf("当前课时费: %d元\n", CalculateFee(&teachers[login_index]));
        }
        else
        {
            printf("当前模式: 访客模式\n");
        }
        printf("系统教师总数: %d\n\n", teacher_count);

        Menu();
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("输入错误\n");
            continue;
        }
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n')
        {
            input[len - 1] = '\0';
            len--;
        }

        if (len == 0)
        {
            printf("请输入选项编号\n");
            printf("按回车键继续...");
            getchar();
            continue;
        }
        int valid = 1;
        for (size_t i = 0; i < len; i++)
        {
            if (!isdigit(input[i]))
            {
                valid = 0;
                break;
            }
        }
        if (!valid)
        {
            printf("错误：请输入数字\n");
            printf("按回车键继续...");
            getchar();
            continue;
        }
        choice = atoi(input);
        switch (choice)
        {
        case 0:
            printf("确定要退出系统吗？(y/n): ");
            if (getchar() == 'y' || getchar() == 'Y')
            {
                if (teachers != NULL)
                    free(teachers);
                printf("感谢使用，再见！\n");
                exit(0);
            }
            while (getchar() != '\n')
                ;
            break;
        case 1:
            if (login_index >= 0 && teachers != NULL)
            {
                AddTeacher(&teachers, &teacher_count);
            }
            else
            {
                printf("错误：需要教师权限才能添加教师\n");
                printf("按回车键继续...");
                getchar();
            }
            break;
        case 2:
            if (login_index >= 0 && teachers != NULL)
            {
                DeleteTeacher(&teachers, &teacher_count, login_index);
            }
            else
            {
                printf("错误：需要教师权限才能删除教师\n");
                printf("按回车键继续...");
                getchar();
            }
            break;
        case 3:
            if (teachers != NULL && teacher_count > 0)
            {
                Sort_Fee(teachers, teacher_count);
            }
            else
            {
                printf("没有可排序的教师数据\n");
                printf("按回车键继续...");
                getchar();
            }
            break;
        case 4:
            if (teachers != NULL && teacher_count > 0)
            {
                Sort_Salary(teachers, teacher_count);
            }
            else
            {
                printf("没有可排序的教师数据\n");
                printf("按回车键继续...");
                getchar();
            }
            break;
        case 5:
        {
            printf("\n=== 系统信息 ===\n");
            printf("当前模式：%s\n", from_teacher ? "教师模式" : "访客模式");
            printf("登录状态：%s\n", login_index >= 0 ? "已登录" : "未登录");

            if (login_index >= 0 && teachers != NULL)
            {
                printf("当前用户：%s\n", teachers[login_index].name);
            }

            printf("教师总数：%d\n", teacher_count);

            int total_fee = 0;
            int max_fee = 0, min_fee = 0;
            char max_name[MAX_NAME_LEN] = "", min_name[MAX_NAME_LEN] = "";

            if (teacher_count > 0)
            {
                for (int i = 0; i < teacher_count; i++)
                {
                    int fee = CalculateFee(&teachers[i]);
                    total_fee += fee;

                    if (i == 0 || fee > max_fee)
                    {
                        max_fee = fee;
                        strcpy(max_name, teachers[i].name);
                    }
                    if (i == 0 || fee < min_fee)
                    {
                        min_fee = fee;
                        strcpy(min_name, teachers[i].name);
                    }
                }

                printf("总课时费：%d元\n", total_fee);
                printf("最高课时费：%s (%d元)\n", max_name, max_fee);
                printf("最低课时费：%s (%d元)\n", min_name, min_fee);
                printf("平均课时费：%.2f元\n", (float)total_fee / teacher_count);
            }

            printf("\n权限信息：\n");
            if (from_teacher)
            {
                printf("✓ 添加/删除教师\n✓ 修改密码\n✓ 保存数据\n✓ 排序功能\n");
            }
            else
            {
                printf("✓ 查看教师信息\n✓ 排序功能\n✗ 修改数据（需要教师登录）\n");
            }

            printf("\n按回车键继续...");
            getchar();
        }
        break;
        case 6:
            if (teachers != NULL && teacher_count > 0)
            {
                SaveTeachersToFile(teachers, teacher_count);
            }
            else
            {
                printf("没有需要保存的数据\n");
                printf("按回车键继续...");
                getchar();
            }
            break;
        case 7:
            if (from_teacher)
            {
                if (teachers != NULL)
                {
                    free(teachers);
                    teachers = NULL;
                }
                system("cls");
                Visitor();
                return;
            }
            else
            {
                printf("您已经在访客模式中\n");
                printf("按回车键继续...");
                getchar();
            }
            break;
        case 8:
            if (!from_teacher)
            {
                if (teachers != NULL)
                {
                    free(teachers);
                    teachers = NULL;
                }
                system("cls");
                Teacher();
                return;
            }
            else
            {
                printf("您已经在教师模式中\n");
                printf("按回车键继续...");
                getchar();
            }
            break;
        case 9:
            if (login_index >= 0 && teachers != NULL)
            {
                ChangePassword(&teachers[login_index], teachers, &teacher_count);
            }
            else
            {
                printf("错误：需要教师登录才能修改密码\n");
                printf("按回车键继续...");
                getchar();
            }
            break;
        case 10:
            if (teachers != NULL && teacher_count > 0)
            {
                Print_all_teachers(teachers, teacher_count);
            }
            else
            {
                printf("没有教师数据可显示\n");
                printf("按回车键继续...");
                getchar();
            }
            break;
        case 11:
            if (teachers != NULL && teacher_count > 0)
            {
                SearchTeacher(teachers, teacher_count);
            }
            else
            {
                printf("没有教师数据可搜索\n");
                printf("按回车键继续...");
                getchar();
            }
            break;
        case 12:
            if (teachers != NULL && teacher_count > 0)
            {
                Statistics(teachers, teacher_count);
            }
            else
            {
                printf("没有数据可统计\n");
                printf("按回车键继续...");
                getchar();
            }
            break;
        case 13:
            if (teachers != NULL && teacher_count > 0)
            {
                ExportToCSV(teachers, teacher_count);
            }
            else
            {
                printf("没有数据可导出\n");
                printf("按回车键继续...");
                getchar();
            }
            break;
        case 14:
        {
            printf("\n=== 数据备份与恢复 ===\n");
            printf("1. 创建备份\n");
            printf("2. 恢复备份\n");
            printf("请选择操作 (1-2): ");

            char backup_choice[10];
            fgets(backup_choice, sizeof(backup_choice), stdin);

            if (atoi(backup_choice) == 1)
            {
                BackupData();
            }
            else if (atoi(backup_choice) == 2)
            {
                RestoreData();
                if (teachers != NULL)
                    free(teachers);
                teachers = NULL;
                teacher_count = 0;

                if (!Teacherinf(&teachers, &teacher_count))
                {
                    printf("重新加载数据失败\n");
                }
            }
            printf("按回车键继续...");
            getchar();
        }
        break;
        default:
            printf("无效选择！请输入0-14之间的数字\n");
            printf("按回车键继续...");
            getchar();
        }
    }
}
void Print_all_teachers(TEACHER *teachers, int teacher_count)
{
    if (teachers == NULL || teacher_count == 0)
    {
        printf("没有教师数据可显示\n");
        return;
    }
    printf("\n");
    printf("========================================================================================================\n");
    printf("                                       教师信息总览                                                   \n");
    printf("========================================================================================================\n");
    printf("序号 姓名        基本工资  课时系数  标准课时  实际课时  超课时    课时费\n");
    printf("--------------------------------------------------------------------------------\n");
    int total_fee = 0;
    for (int i = 0; i < teacher_count; i++)
    {
        int overtime = teachers[i].Time > teachers[i].Standard ? teachers[i].Time - teachers[i].Standard : 0;
        int fee = CalculateFee(&teachers[i]);
        total_fee += fee;
        printf("%-4d %-10s %-9d %-9d %-9d %-9d %-9d %-9d\n",
               i + 1,
               teachers[i].name,
               teachers[i].Salary,
               teachers[i].Coefficient,
               teachers[i].Standard,
               teachers[i].Time,
               overtime,
               fee);
    }
    printf("================================================================================\n");
    printf("总计：%d 名教师，总课时费：%d 元\n", teacher_count, total_fee);
    printf("================================================================================\n");
    printf("\n按回车键返回菜单...");
    getchar();
    getchar();
}
int CalculateFee(TEACHER *teacher)
{
    if (teacher == NULL)
        return 0;
    int overtime = teacher->Time > teacher->Standard ? teacher->Time - teacher->Standard : 0;
    return overtime * teacher->Coefficient * teacher->Salary;
}
void SaveTeachersToFile(TEACHER *teachers, int teacher_count)
{
    BackupData();
    FILE *file = fopen(INF_FILE, "w");
    if (file == NULL)
    {
        printf("无法打开文件进行写入\n");
        return;
    }
    fprintf(file, "# 教师信息文件\n");
    fprintf(file, "# 格式：姓名 密码 基本工资 课时系数 标准课时 实际课时\n");
    fprintf(file, "# 创建时间：%s\n", __DATE__);
    fprintf(file, "#\n");
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
    printf("教师信息保存成功！\n");
    int total_fee = 0;
    for (int i = 0; i < teacher_count; i++)
    {
        total_fee += CalculateFee(&teachers[i]);
    }
    printf("教师总数：%d，总课时费：%d 元\n", teacher_count, total_fee);
}
void ChangePassword(TEACHER *teacher, TEACHER *teachers, int *teacher_count)
{
    if (teacher == NULL)
        return;
    char old_password[PASSWORD_LEN];
    char new_password[PASSWORD_LEN];
    char confirm_password[PASSWORD_LEN];
    printf("\n=== 修改密码 ===\n");
    printf("当前用户：%s\n", teacher->name);
    int attempts = 3;
    while (attempts > 0)
    {
        printf("请输入旧密码（剩余尝试次数：%d）：", attempts);
        GetPass(old_password, PASSWORD_LEN);
        if (strcmp(old_password, teacher->password) == 0)
        {
            break;
        }

        attempts--;
        if (attempts > 0)
        {
            printf("密码错误，请重试\n");
        }
    }
    if (attempts == 0)
    {
        printf("错误：密码验证失败\n");
        printf("按回车键继续...");
        getchar();
        return;
    }
    printf("请输入新密码（至少4位）：");
    GetPass(new_password, PASSWORD_LEN);
    if (strlen(new_password) < 4)
    {
        printf("错误：密码长度至少为4位\n");
        printf("按回车键继续...");
        getchar();
        return;
    }
    if (strcmp(new_password, old_password) == 0)
    {
        printf("错误：新密码不能与旧密码相同\n");
        printf("按回车键继续...");
        getchar();
        return;
    }
    printf("请确认新密码：");
    GetPass(confirm_password, PASSWORD_LEN);
    if (strcmp(new_password, confirm_password) != 0)
    {
        printf("错误：两次输入的新密码不一致\n");
        printf("按回车键继续...");
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
    printf("密码修改成功！\n");
    printf("按回车键继续...");
    getchar();
}
int CompareTeachers(const TEACHER *a, const TEACHER *b, int sort_by)
{
    switch (sort_by)
    {
    case 1:
        return a->Salary - b->Salary;
    case 2:
        return CalculateFee((TEACHER *)b) - CalculateFee((TEACHER *)a);
    default:
        return 0;
    }
}
void QuickSort(TEACHER *teachers, int left, int right, int sort_by)
{
    if (left >= right)
        return;

    int i = left, j = right;
    TEACHER pivot = teachers[(left + right) / 2];
    while (i <= j)
    {
        while (CompareTeachers(&teachers[i], &pivot, sort_by) < 0)
            i++;
        while (CompareTeachers(&teachers[j], &pivot, sort_by) > 0)
            j--;

        if (i <= j)
        {
            TEACHER temp = teachers[i];
            teachers[i] = teachers[j];
            teachers[j] = temp;
            i++;
            j--;
        }
    }
    if (left < j)
        QuickSort(teachers, left, j, sort_by);
    if (i < right)
        QuickSort(teachers, i, right, sort_by);
}
void Sort_Salary(TEACHER *teachers, int teacher_count)
{
    if (teachers == NULL || teacher_count == 0)
    {
        printf("没有教师数据可排序\n");
        return;
    }
    printf("\n=== 按基本工资排序 ===\n");
    printf("正在排序...\n");
    TEACHER *sorted_teachers = (TEACHER *)malloc(teacher_count * sizeof(TEACHER));
    if (sorted_teachers == NULL)
    {
        printf("内存分配失败\n");
        return;
    }
    memcpy(sorted_teachers, teachers, teacher_count * sizeof(TEACHER));
    QuickSort(sorted_teachers, 0, teacher_count - 1, 1);
    printf("排序完成！\n\n");
    printf("========================================================================================================\n");
    printf("                           按基本工资排序（升序）                                                     \n");
    printf("========================================================================================================\n");
    printf("排名 姓名        基本工资  课时系数  标准课时  实际课时  超课时    课时费\n");
    printf("--------------------------------------------------------------------------------\n");
    int total_fee = 0;
    for (int i = 0; i < teacher_count; i++)
    {
        int overtime = sorted_teachers[i].Time > sorted_teachers[i].Standard ? sorted_teachers[i].Time - sorted_teachers[i].Standard : 0;
        int fee = CalculateFee(&sorted_teachers[i]);
        total_fee += fee;
        printf("%-4d %-10s %-9d %-9d %-9d %-9d %-9d %-9d\n",
               i + 1,
               sorted_teachers[i].name,
               sorted_teachers[i].Salary,
               sorted_teachers[i].Coefficient,
               sorted_teachers[i].Standard,
               sorted_teachers[i].Time,
               overtime,
               fee);
    }
    printf("================================================================================\n");
    printf("最低工资：%d 元，最高工资：%d 元，平均工资：%.2f 元\n",
           sorted_teachers[0].Salary,
           sorted_teachers[teacher_count - 1].Salary,
           (float)total_fee / teacher_count);
    free(sorted_teachers);
    printf("\n是否保存排序结果到文件？(y/n): ");
    char choice;
    scanf(" %c", &choice);
    while (getchar() != '\n')
        ;
    if (choice == 'y' || choice == 'Y')
    {
        SaveTeachersToFile(teachers, teacher_count);
    }
    printf("\n按回车键继续...");
    getchar();
}
void Sort_Fee(TEACHER *teachers, int teacher_count)
{
    if (teachers == NULL || teacher_count == 0)
    {
        printf("没有教师数据可排序\n");
        return;
    }
    printf("\n=== 按课时费排序 ===\n");
    printf("正在排序...\n");
    TEACHER *sorted_teachers = (TEACHER *)malloc(teacher_count * sizeof(TEACHER));
    if (sorted_teachers == NULL)
    {
        printf("内存分配失败\n");
        return;
    }
    memcpy(sorted_teachers, teachers, teacher_count * sizeof(TEACHER));
    QuickSort(sorted_teachers, 0, teacher_count - 1, 2);
    printf("排序完成！\n\n");
    printf("========================================================================================================\n");
    printf("                           按课时费排序（降序）                                                       \n");
    printf("========================================================================================================\n");
    printf("排名 姓名        基本工资  课时系数  标准课时  实际课时  超课时    课时费\n");
    printf("--------------------------------------------------------------------------------\n");
    int total_fee = 0;
    int max_fee = 0, min_fee = 0;
    for (int i = 0; i < teacher_count; i++)
    {
        int overtime = sorted_teachers[i].Time > sorted_teachers[i].Standard ? sorted_teachers[i].Time - sorted_teachers[i].Standard : 0;
        int fee = CalculateFee(&sorted_teachers[i]);
        total_fee += fee;
        if (i == 0)
            max_fee = fee;
        if (i == teacher_count - 1)
            min_fee = fee;
        char rank[8] = "";
        if (i < 3)
        {
            sprintf(rank, "第%d名", i + 1);
        }
        printf("%-4s %-10s %-9d %-9d %-9d %-9d %-9d %-9d\n",
               rank,
               sorted_teachers[i].name,
               sorted_teachers[i].Salary,
               sorted_teachers[i].Coefficient,
               sorted_teachers[i].Standard,
               sorted_teachers[i].Time,
               overtime,
               fee);
    }
    printf("================================================================================\n");
    printf("最高课时费：%d 元，最低课时费：%d 元，平均课时费：%.2f 元\n",
           max_fee, min_fee, (float)total_fee / teacher_count);
    printf("\n=== 课时费前三名 ===\n");
    for (int i = 0; i < teacher_count && i < 3; i++)
    {
        int fee = CalculateFee(&sorted_teachers[i]);
        printf("第%d名：%s，课时费：%d 元\n",
               i + 1, sorted_teachers[i].name, fee);
    }
    free(sorted_teachers);
    printf("\n是否保存排序结果到文件？(y/n): ");
    char choice;
    scanf(" %c", &choice);
    while (getchar() != '\n')
        ;
    if (choice == 'y' || choice == 'Y')
    {
        SaveTeachersToFile(teachers, teacher_count);
    }
    printf("\n按回车键继续...");
    getchar();
}
void AddTeacher(TEACHER **teachers_ptr, int *teacher_count_ptr)
{
    if (*teachers_ptr == NULL)
    {
        printf("错误：教师信息未加载\n");
        return;
    }
    printf("\n=== 添加新教师 ===\n");
    TEACHER new_teacher;
    memset(&new_teacher, 0, sizeof(TEACHER));
    while (1)
    {
        printf("请输入教师姓名：");
        if (fgets(new_teacher.name, sizeof(new_teacher.name), stdin) == NULL)
        {
            printf("输入错误\n");
            return;
        }
        size_t len = strlen(new_teacher.name);
        if (len > 0 && new_teacher.name[len - 1] == '\n')
        {
            new_teacher.name[len - 1] = '\0';
            len--;
        }
        if (len == 0)
        {
            printf("错误：姓名不能为空\n");
            continue;
        }
        if (len >= MAX_NAME_LEN - 1)
        {
            printf("错误：姓名过长\n");
            continue;
        }
        int exists = 0;
        for (int i = 0; i < *teacher_count_ptr; i++)
        {
            if (strcmp((*teachers_ptr)[i].name, new_teacher.name) == 0)
            {
                printf("错误：教师 '%s' 已存在\n", new_teacher.name);
                exists = 1;
                break;
            }
        }
        if (!exists)
            break;
    }
    char password1[PASSWORD_LEN], password2[PASSWORD_LEN];
    while (1)
    {
        printf("请输入密码（至少4位）：");
        GetPass(password1, PASSWORD_LEN);

        if (strlen(password1) < 4)
        {
            printf("错误：密码长度至少为4位\n");
            continue;
        }
        printf("请确认密码：");
        GetPass(password2, PASSWORD_LEN);

        if (strcmp(password1, password2) != 0)
        {
            printf("错误：两次输入的密码不一致\n");
            continue;
        }
        strcpy(new_teacher.password, password1);
        break;
    }
    while (1)
    {
        printf("请输入基本工资（正整数）：");
        char input[32];
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("输入错误\n");
            return;
        }
        new_teacher.Salary = atoi(input);
        if (new_teacher.Salary <= 0)
        {
            printf("错误：工资必须是正整数\n");
            continue;
        }
        break;
    }
    while (1)
    {
        printf("请输入课时系数（正整数）：");
        char input[32];
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("输入错误\n");
            return;
        }
        new_teacher.Coefficient = atoi(input);
        if (new_teacher.Coefficient <= 0)
        {
            printf("错误：课时系数必须是正整数\n");
            continue;
        }
        break;
    }

    while (1)
    {
        printf("请输入标准课时（非负整数）：");
        char input[32];
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("输入错误\n");
            return;
        }

        new_teacher.Standard = atoi(input);
        if (new_teacher.Standard < 0)
        {
            printf("错误：标准课时不能为负数\n");
            continue;
        }
        break;
    }

    while (1)
    {
        printf("请输入实际课时（非负整数）：");
        char input[32];
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("输入错误\n");
            return;
        }

        new_teacher.Time = atoi(input);
        if (new_teacher.Time < 0)
        {
            printf("错误：实际课时不能为负数\n");
            continue;
        }
        break;
    }

    if (new_teacher.Time < new_teacher.Standard)
    {
        printf("警告：实际课时（%d）小于标准课时（%d）\n",
               new_teacher.Time, new_teacher.Standard);
        printf("该教师将没有超课时费\n");
        printf("是否继续？(y/n): ");

        char confirm;
        scanf(" %c", &confirm);
        while (getchar() != '\n')
            ;

        if (confirm != 'y' && confirm != 'Y')
        {
            printf("添加取消\n");
            return;
        }
    }

    int new_count = *teacher_count_ptr + 1;
    TEACHER *new_teachers = (TEACHER *)realloc(*teachers_ptr, new_count * sizeof(TEACHER));
    if (new_teachers == NULL)
    {
        printf("错误：内存分配失败\n");
        return;
    }

    new_teachers[*teacher_count_ptr] = new_teacher;
    *teachers_ptr = new_teachers;
    *teacher_count_ptr = new_count;
    SaveTeachersToFile(*teachers_ptr, *teacher_count_ptr);
    printf("\n=== 教师添加成功 ===\n");
    printf("姓名：%s\n", new_teacher.name);
    printf("基本工资：%d\n", new_teacher.Salary);
    printf("课时系数：%d\n", new_teacher.Coefficient);
    printf("标准课时：%d\n", new_teacher.Standard);
    printf("实际课时：%d\n", new_teacher.Time);
    int overtime = new_teacher.Time > new_teacher.Standard ? new_teacher.Time - new_teacher.Standard : 0;
    int fee = overtime * new_teacher.Coefficient * new_teacher.Salary;
    printf("超课时：%d\n", overtime);
    printf("课时费：%d元\n", fee);
    printf("=============================\n");
    printf("按回车键继续...");
    getchar();
}
void DeleteTeacher(TEACHER **teachers_ptr, int *teacher_count_ptr, int current_login_index)
{
    if (*teachers_ptr == NULL || *teacher_count_ptr == 0)
    {
        printf("错误：没有教师数据\n");
        return;
    }
    printf("\n=== 删除教师 ===\n");
    Print_all_teachers(*teachers_ptr, *teacher_count_ptr);
    char teacher_name[MAX_NAME_LEN];
    printf("\n请输入要删除的教师姓名：");
    if (fgets(teacher_name, sizeof(teacher_name), stdin) == NULL)
    {
        printf("输入错误\n");
        return;
    }
    size_t len = strlen(teacher_name);
    if (len > 0 && teacher_name[len - 1] == '\n')
    {
        teacher_name[len - 1] = '\0';
        len--;
    }
    if (len == 0)
    {
        printf("错误：教师姓名不能为空\n");
        return;
    }
    if (current_login_index >= 0 &&
        strcmp((*teachers_ptr)[current_login_index].name, teacher_name) == 0)
    {
        printf("错误：不能删除当前登录的账户\n");
        printf("请让其他教师或管理员删除此账户\n");
        printf("按回车键继续...");
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
        printf("错误：教师 '%s' 不存在\n", teacher_name);
        printf("按回车键继续...");
        getchar();
        return;
    }
    printf("\n=== 确认删除教师信息 ===\n");
    printf("姓名：%s\n", (*teachers_ptr)[delete_index].name);
    printf("基本工资：%d\n", (*teachers_ptr)[delete_index].Salary);
    printf("课时系数：%d\n", (*teachers_ptr)[delete_index].Coefficient);
    printf("标准课时：%d\n", (*teachers_ptr)[delete_index].Standard);
    printf("实际课时：%d\n", (*teachers_ptr)[delete_index].Time);
    printf("课时费：%d元\n", CalculateFee(&(*teachers_ptr)[delete_index]));
    printf("===============================\n");
    printf("\n确定要删除教师 '%s' 吗？(y/n): ", teacher_name);
    char confirm;
    scanf(" %c", &confirm);
    while (getchar() != '\n')
        ;
    if (confirm != 'y' && confirm != 'Y')
    {
        printf("删除取消\n");
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
        for (int i = delete_index; i < *teacher_count_ptr - 1; i++)
        {
            (*teachers_ptr)[i] = (*teachers_ptr)[i + 1];
        }
        (*teacher_count_ptr)--;
        TEACHER *temp = (TEACHER *)realloc(*teachers_ptr, (*teacher_count_ptr) * sizeof(TEACHER));
        if (temp != NULL || *teacher_count_ptr == 0)
        {
            *teachers_ptr = temp;
        }
    }
    SaveTeachersToFile(*teachers_ptr, *teacher_count_ptr);
    printf("\n=== 教师删除成功 ===\n");
    printf("教师 '%s' 已从系统中删除\n", teacher_name);
    printf("剩余教师数量：%d\n", *teacher_count_ptr);
    printf("按回车键继续...");
    getchar();
}
void BackupData()
{
    FILE *source = fopen(INF_FILE, "rb");
    if (source == NULL)
        return;
    FILE *dest = fopen(BACKUP_FILE, "wb");
    if (dest == NULL)
    {
        fclose(source);
        return;
    }
    char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), source)) > 0)
    {
        fwrite(buffer, 1, bytes, dest);
    }
    fclose(source);
    fclose(dest);
    printf("数据备份已创建：%s\n", BACKUP_FILE);
}
void RestoreData()
{
    printf("警告：这将覆盖当前数据！\n");
    printf("确定要恢复备份吗？(y/n): ");
    char confirm;
    scanf(" %c", &confirm);
    while (getchar() != '\n')
        ;
    if (confirm != 'y' && confirm != 'Y')
    {
        printf("恢复取消\n");
        return;
    }
    FILE *source = fopen(BACKUP_FILE, "rb");
    if (source == NULL)
    {
        printf("错误：备份文件不存在\n");
        return;
    }
    FILE *dest = fopen(INF_FILE, "wb");
    if (dest == NULL)
    {
        fclose(source);
        return;
    }
    char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), source)) > 0)
    {
        fwrite(buffer, 1, bytes, dest);
    }
    fclose(source);
    fclose(dest);
    printf("数据恢复成功\n");
}
void ExportToCSV(TEACHER *teachers, int teacher_count)
{
    char filename[256];
    sprintf(filename, "teachers_export_%s.csv", __DATE__);
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("无法创建导出文件\n");
        return;
    }
    fprintf(file, "序号,姓名,基本工资,课时系数,标准课时,实际课时,超课时,课时费\n");
    for (int i = 0; i < teacher_count; i++)
    {
        int overtime = teachers[i].Time > teachers[i].Standard ? teachers[i].Time - teachers[i].Standard : 0;
        int fee = CalculateFee(&teachers[i]);
        fprintf(file, "%d,%s,%d,%d,%d,%d,%d,%d\n",
                i + 1,
                teachers[i].name,
                teachers[i].Salary,
                teachers[i].Coefficient,
                teachers[i].Standard,
                teachers[i].Time,
                overtime,
                fee);
    }
    fclose(file);
    printf("数据已导出到文件：%s\n", filename);
    printf("可以使用Excel或其他表格软件打开此文件\n");
    printf("按回车键继续...");
    getchar();
}
void SearchTeacher(TEACHER *teachers, int teacher_count)
{
    char keyword[MAX_NAME_LEN];
    printf("\n=== 搜索教师 ===\n");
    printf("请输入搜索关键词（姓名或部分姓名）：");
    if (fgets(keyword, sizeof(keyword), stdin) == NULL)
    {
        printf("输入错误\n");
        return;
    }
    size_t len = strlen(keyword);
    if (len > 0 && keyword[len - 1] == '\n')
    {
        keyword[len - 1] = '\0';
        len--;
    }
    if (len == 0)
    {
        printf("错误：搜索关键词不能为空\n");
        return;
    }
    printf("\n搜索结果：\n");
    printf("----------------------------------------------------------------\n");
    printf("姓名        基本工资  课时系数  标准课时  实际课时  课时费\n");
    printf("----------------------------------------------------------------\n");
    int found = 0;
    for (int i = 0; i < teacher_count; i++)
    {
        if (strstr(teachers[i].name, keyword) != NULL)
        {
            int fee = CalculateFee(&teachers[i]);
            printf("%-10s %-9d %-9d %-9d %-9d %-9d\n",
                   teachers[i].name,
                   teachers[i].Salary,
                   teachers[i].Coefficient,
                   teachers[i].Standard,
                   teachers[i].Time,
                   fee);
            found++;
        }
    }
    if (found == 0)
    {
        printf("未找到匹配的教师\n");
    }
    else
    {
        printf("----------------------------------------------------------------\n");
        printf("共找到 %d 名教师\n", found);
    }
    printf("\n按回车键继续...");
    getchar();
}
void Statistics(TEACHER *teachers, int teacher_count)
{
    if (teachers == NULL || teacher_count == 0)
    {
        printf("没有数据可统计\n");
        return;
    }
    printf("\n=== 数据统计 ===\n");
    int total_salary = 0;
    int total_fee = 0;
    int total_overtime = 0;
    int max_fee = 0, min_fee = 0;
    int max_salary = 0, min_salary = 0;
    char max_fee_name[MAX_NAME_LEN] = "", min_fee_name[MAX_NAME_LEN] = "";
    char max_salary_name[MAX_NAME_LEN] = "", min_salary_name[MAX_NAME_LEN] = "";
    for (int i = 0; i < teacher_count; i++)
    {
        int salary = teachers[i].Salary;
        int fee = CalculateFee(&teachers[i]);
        int overtime = teachers[i].Time > teachers[i].Standard ? teachers[i].Time - teachers[i].Standard : 0;
        total_salary += salary;
        total_fee += fee;
        total_overtime += overtime;
        if (i == 0 || fee > max_fee)
        {
            max_fee = fee;
            strcpy(max_fee_name, teachers[i].name);
        }
        if (i == 0 || fee < min_fee)
        {
            min_fee = fee;
            strcpy(min_fee_name, teachers[i].name);
        }
        if (i == 0 || salary > max_salary)
        {
            max_salary = salary;
            strcpy(max_salary_name, teachers[i].name);
        }
        if (i == 0 || salary < min_salary)
        {
            min_salary = salary;
            strcpy(min_salary_name, teachers[i].name);
        }
    }
    printf("教师总数：%d\n", teacher_count);
    printf("总基本工资：%d元\n", total_salary);
    printf("总课时费：%d元\n", total_fee);
    printf("总超课时：%d课时\n", total_overtime);
    printf("\n");
    printf("平均基本工资：%.2f元\n", (float)total_salary / teacher_count);
    printf("平均课时费：%.2f元\n", (float)total_fee / teacher_count);
    printf("平均超课时：%.2f课时\n", (float)total_overtime / teacher_count);
    printf("\n");
    printf("最高基本工资：%s（%d元）\n", max_salary_name, max_salary);
    printf("最低基本工资：%s（%d元）\n", min_salary_name, min_salary);
    printf("最高课时费：%s（%d元）\n", max_fee_name, max_fee);
    printf("最低课时费：%s（%d元）\n", min_fee_name, min_fee);
    printf("\n=== 课时费分布 ===\n");
    int brackets[5] = {0};
    for (int i = 0; i < teacher_count; i++)
    {
        int fee = CalculateFee(&teachers[i]);
        if (fee <= 1000)
            brackets[0]++;
        else if (fee <= 3000)
            brackets[1]++;
        else if (fee <= 5000)
            brackets[2]++;
        else if (fee <= 7000)
            brackets[3]++;
        else
            brackets[4]++;
    }
    printf("0-1000元：%d人（%.1f%%）\n", brackets[0], (float)brackets[0] / teacher_count * 100);
    printf("1001-3000元：%d人（%.1f%%）\n", brackets[1], (float)brackets[1] / teacher_count * 100);
    printf("3001-5000元：%d人（%.1f%%）\n", brackets[2], (float)brackets[2] / teacher_count * 100);
    printf("5001-7000元：%d人（%.1f%%）\n", brackets[3], (float)brackets[3] / teacher_count * 100);
    printf("7000元以上：%d人（%.1f%%）\n", brackets[4], (float)brackets[4] / teacher_count * 100);
    printf("\n按回车键继续...");
    getchar();
}