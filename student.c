#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MaxStack 6 //กำหนดขนาด stack สำหรับเก็บ operation ล่าสุด (เก็บได้ไม่เกิน 5 รายการ)  
#define INSERT 1 //กำหนดค่าคงที่สำหรับ operation insert
#define DELETE 2 //กำหนดค่าคงที่สำหรับ operation delete



//โครงสร้างข้อมูลที่ต้องใช้ 
struct Student { 
    char id[10]; 
    char name[50]; 
    float gpa; 

};




//โครงสร้างข้อมูลที่ต้องใช้
struct DNode { struct Student data;
struct DNode *prev;
struct DNode *next; };



//กำหนดโครงสร้างสำหรับเก็บ operation ที่ทำล่าสุด
struct Operation 
{
    int type;              
    struct Student data;   
};




//Stack สำหรับเก็บ operation ล่าสุด
struct DNode *H = NULL, *H1, *p; 
struct Operation stack[MaxStack];
int SP = 0;                      



//Prototype + การทำงานของฟังก์ชัน
struct DNode *Allocate(); //Allocate หน่วยความจำสำหรับ node ใหม่

void push(int type, struct Student s); //ดัน operation ล่าสุดลง stack (ถ้าเต็มแล้วให้ทิ้งรายการเก่าสุด)

void addNode(struct DNode **head, struct Student student); //ป้อน node ใหม่ลง linked list (เรียงตาม GPA มากไปน้อย)

int removeNode(struct DNode **head, char *id); //ลบ node ออกจาก linked list (ไม่ push stack) คืน 1 ถ้าลบได้

struct DNode *findById(struct DNode *head, char *id); //หา node ตามรหัสนักศึกษา (ไม่ push stack) คืน pointer ของ node ถ้าเจอ

void insertSorted(struct DNode **head, struct Student student); //แทรก node ใหม่ลงใน linked list ตามลำดับ GPA และ push INSERT ลง stack

void deleteById(struct DNode **head, char *id); //ลบ node ตามรหัสนักศึกษา และ push DELETE ลง stack

void printForward(struct DNode *head); //แสดงรายการจากต้นไปท้าย

void printBackward(struct DNode *head); //แสดงรายการจากท้ายไปต้น

void undo(struct DNode **head, struct Operation stack[]); //ยกเลิกการกระทำล่าสุด (insert -> ลบออก, delete -> ใส่กลับ)

void freeAll(struct DNode **head); //ปลดหน่วยความจำทั้งหมดก่อนจบโปรแกรม

struct DNode *Allocate() //Allocate หน่วยความจำสำหรับ node ใหม่
{
    struct DNode *temp;
    temp = (struct DNode *)malloc(sizeof(struct DNode));
    return(temp);
}
//End




//ฟังก์ชัน Stack 
void push(int type, struct Student s) // PUSH Function (จำกัดไม่เกิน 5 รายการ index 1..5)
{
    int k;
    if (SP == MaxStack - 1) // เมื่อ Stack เต็ม 5 รายการ (SP = 5)
    {
        // เลื่อนข้อมูลจากตำแหน่ง 2..SP ไปทับตำแหน่ง 1..(SP-1) เพื่อทิ้งตัวที่เก่าที่สุด (index 1)
        for (k = 1; k < SP; k++)
        {
            stack[k] = stack[k + 1];
        }
        SP--; // ลด SP ลง 1 ตำแหน่งเพื่อให้พร้อมรับข้อมูลใหม่ (SP จะเป็น 4)
    }
    SP = SP + 1; // เพิ่ม SP ไปยังตำแหน่งว่างถัดไป
    stack[SP].type = type;
    stack[SP].data = s;
    // หลังจาก push SP จะอยู่ในช่วง 1..5 เสมอ (ไม่เกิน MaxStack-1 = 5)
}




// Linked List (ไม่มีการดันถึง Stack)
void addNode(struct DNode **head, struct Student student) //ป้อนด้วย GPA high ไป low
{
    p = Allocate();
    if (p == NULL)
    {
        printf("Out of memory!!\n");
        return;
    }
    p->data = student;

    if (*head == NULL || student.gpa > (*head)->data.gpa) //ป้อนที่โหนดแรก
    {
        p->prev = NULL;
        p->next = *head;
        if (*head != NULL)
            (*head)->prev = p;
        *head = p;
    }
    else
    {
        H1 = *head; //หาโนดที่ป้อนหลังจากโนดที่ GPA มากกว่า
        while (H1->next != NULL && H1->next->data.gpa >= student.gpa)
            H1 = H1->next;
        p->next = H1->next;
        p->prev = H1;
        if (H1->next != NULL) //ถ้า H1 ไม่ใช่โหนดสุดท้าย
            H1->next->prev = p;
        H1->next = p;
    }
}




//หา node ตาม id (ไม่ push stack) คืน pointer ของ node ถ้าเจอ
struct DNode *findById(struct DNode *head, char *id) //หา node ตามรหัสนักศึกษา
{
    H1 = head;
    while (H1 != NULL)
    {
        if (strcmp(H1->data.id, id) == 0) //ถ้าเจอ id ที่ต้องการ
            return(H1);
        H1 = H1->next;
    }
    return(NULL);
}




//ลบ node ออกจากลิสต์ (ไม่ push stack) คืน 1 ถ้าลบได้
int removeNode(struct DNode **head, char *id) //Delete node by id
{
    p = findById(*head, id);
    if (p == NULL)
        return(0);

    if (p->prev == NULL && p->next == NULL) //โหนดเดียวในลิสต์ (only one node)
        *head = NULL;
    else if (p->prev == NULL) //โหนดแรก (head node)
    {
        *head = p->next;
        (*head)->prev = NULL;
    }
    else if (p->next == NULL) //โหนดสุดท้าย (tail node)
        p->prev->next = NULL;
    else //โหนดกลาง (middle node)
    {
        p->prev->next = p->next;
        p->next->prev = p->prev;
    }
    free(p); //ปลดหน่วยความจำของ node ที่ลบออก
    return(1);
}




//5 ฟังก์ชันตามโจทย์

// insertSorted: แทรก node ใหม่เรียงตาม GPA (มากไปน้อย) แล้ว push INSERT ลง stack
void insertSorted(struct DNode **head, struct Student student)
{
    // แทรก node เรียงตาม GPA มากไปน้อย
    p = Allocate();
    if (p == NULL)
    {
        printf("Out of memory!!\n");
        return;
    }
    p->data = student;

    if (*head == NULL || student.gpa > (*head)->data.gpa) //แทรกเป็น head
    {
        p->prev = NULL;
        p->next = *head;
        if (*head != NULL)
            (*head)->prev = p;
        *head = p;
    }
    else
    {
        H1 = *head;
        while (H1->next != NULL && H1->next->data.gpa >= student.gpa)
            H1 = H1->next;
        p->next = H1->next;
        p->prev = H1;
        if (H1->next != NULL)
            H1->next->prev = p;
        H1->next = p;
    }

    // push INSERT operation ลง stack เพื่อรองรับ undo
    push(INSERT, student);
}




// deleteById: ค้นหา node ด้วย id แล้วลบออกจากลิสต์ทุกกรณี (head/tail/กลาง) และ push DELETE ลง stack
void deleteById(struct DNode **head, char *id)
{
    struct Student temp;
    struct DNode *target;

    target = findById(*head, id); //ค้นหา node ตาม id
    if (target == NULL)
    {
        printf("Student id %s not found!!\n", id);
        return;
    }
    temp = target->data; //เก็บข้อมูลไว้ก่อน free

    // ลบออกจาก linked list ทุกกรณี
    if (target->prev == NULL && target->next == NULL) //โหนดเดียวในลิสต์
        *head = NULL;
    else if (target->prev == NULL) //โหนดแรก (head)
    {
        *head = target->next;
        (*head)->prev = NULL;
    }
    else if (target->next == NULL) //โหนดสุดท้าย (tail)
        target->prev->next = NULL;
    else //โหนดกลาง (middle)
    {
        target->prev->next = target->next;
        target->next->prev = target->prev;
    }
    free(target); //ปลดหน่วยความจำ

    // push DELETE operation ลง stack เพื่อรองรับ undo
    push(DELETE, temp);
    printf("Deleted id %s\n", id);
}




//ฟังก์ชั่นแสดงรายการไปข้างหน้า
void printForward(struct DNode *head)
{
    H1 = head;
    printf("Forward: ");
    while (H1 != NULL)
    {
        printf("[%s|%s|%.2f] -> ", H1->data.id, H1->data.name, H1->data.gpa);
        H1 = H1->next;
    }
    printf("NULL\n");
}




//ฟังก์ชั่นแสดงรายการย้อนกลับ
void printBackward(struct DNode *head)
{
    H1 = head;
    printf("Backward: ");
    if (H1 != NULL)
    {
        while (H1->next != NULL) //เดินไปถึง node สุดท้าย
            H1 = H1->next;
        while (H1 != NULL)
        {
            printf("[%s|%s|%.2f] -> ", H1->data.id, H1->data.name, H1->data.gpa);
            H1 = H1->prev;
        }
    }
    printf("NULL\n");
}




//ฟังก์ชั่น undo: ถ้า operation ล่าสุดเป็น INSERT ให้ลบออก, ถ้าเป็น DELETE ให้ใส่กลับ
void undo(struct DNode **head, struct Operation stack[])
{
    struct Operation x;
    struct DNode *target;

    if (SP == 0) //Stack ว่าง (underflow)
    {
        printf("!!!UNDER FLOW!!!... nothing to undo\n");
        return;
    }
    x = stack[SP]; //อ่าน operation ล่าสุดจากยอด stack
    SP--;           //pop ออก

    if (x.type == INSERT) //undo INSERT -> ลบ node นั้นออก
    {
        // ค้นหาแล้วลบออกจากลิสต์ทุกกรณี (head/tail/กลาง)
        target = findById(*head, x.data.id);
        if (target != NULL)
        {
            if (target->prev == NULL && target->next == NULL)
                *head = NULL;
            else if (target->prev == NULL)
            {
                *head = target->next;
                (*head)->prev = NULL;
            }
            else if (target->next == NULL)
                target->prev->next = NULL;
            else
            {
                target->prev->next = target->next;
                target->next->prev = target->prev;
            }
            free(target);
        }
        printf("Undo: ยกเลิกการเพิ่ม %s\n", x.data.id);
    }
    else //undo DELETE -> ใส่ node กลับเข้าลิสต์เรียงตาม GPA
    {
        // แทรก node กลับเรียงตาม GPA มากไปน้อย (ไม่ push stack ซ้ำ)
        p = Allocate();
        if (p != NULL)
        {
            p->data = x.data;
            if (*head == NULL || x.data.gpa > (*head)->data.gpa)
            {
                p->prev = NULL;
                p->next = *head;
                if (*head != NULL)
                    (*head)->prev = p;
                *head = p;
            }
            else
            {
                H1 = *head;
                while (H1->next != NULL && H1->next->data.gpa >= x.data.gpa)
                    H1 = H1->next;
                p->next = H1->next;
                p->prev = H1;
                if (H1->next != NULL)
                    H1->next->prev = p;
                H1->next = p;
            }
        }
        printf("Undo: ยกเลิกการลบ %s\n", x.data.id);
    }
    printForward(*head);
}




//ปลดหน่วยความจำทั้งหมดก่อนจบโปรแกรม
void freeAll(struct DNode **head)
{
    while (*head != NULL)
    {
        p = *head;
        *head = p->next;
        free(p);
    }
}




//ฟังก์ main() ของโปรแกรม
int main()
{
    struct Student s;
    char id[10];
    int menu;

    do
    {
        printf("\n===== Student Management =====\n");
        printf("1. เพิ่มนักศึกษา\n");
        printf("2. ลบนักศึกษา (ระบุรหัส)\n");
        printf("3. แสดงรายชื่อ (ไปข้างหน้า)\n");
        printf("4. แสดงรายชื่อ (ย้อนกลับ)\n");
        printf("5. Undo การกระทำล่าสุด\n");
        printf("0. ออกจากโปรแกรม\n");
        printf("MENU : ");

        if (scanf("%d", &menu) != 1) //ไม่ใช่ตัวเลข
        {
            while (getchar() != '\n');
            menu = -1;
        }

        switch (menu)
        {
        case 1: printf("\nID : ");
                scanf("%9s", s.id);
                if (findById(H, s.id) != NULL)
                {
                    printf("This id already exist!!\n");
                    break;
                }
                printf("Name : ");
                scanf("%49s", s.name);
                printf("GPA : ");
                scanf("%f", &s.gpa);
                insertSorted(&H, s);
                printf("เพิ่ม: %s %s GPA=%.2f\n", s.id, s.name, s.gpa);
                break;
        case 2: printf("\nDelete ID : ");
                scanf("%9s", id);
                deleteById(&H, id);
                break;
        case 3: printForward(H);
                break;
        case 4: printBackward(H);
                break;
        case 5: undo(&H, stack);
                break;
        case 0: printf("Bye..\n");
                break;
        default: printf("Wrong menu!!\n");
        } //End SWITCH CASE
    } while (menu != 0); //จบ DO WHILE Loop


    freeAll(&H);//ปลดหน่วยความจำทั้งหมดก่อนจบโปรแกรม
    return(0);
}//จบ MAIN ฟังก์ชั่น
