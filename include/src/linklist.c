#include <stdio.h>
#include <stdlib.h>
#include "linklist.h"

/*Sorting: Merge sort*/
struct listnode* list_split(struct listnode* head) {
    if (head == NULL) return head;
    struct listnode* slow = head;
    struct listnode* fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

struct listnode* list_merge(struct list *_list, struct listnode* first, struct listnode* second) {
    if (!first) return second;
    if (!second) return first;
    int32_t cmp = _list->cmp(first->data, second->data);
    if (cmp <= 0) {
        first->next = list_merge(_list, first->next, second);
        if (first->next) first->next->prev = first;
        first->prev = NULL;
        return first;
    } else {
        second->next = list_merge(_list, first, second->next);
        if (second->next) second->next->prev = second;
        second->prev = NULL;
        return second;
    }
}

void list_merge_sort(struct list *_list, struct listnode** head_ref) {
    struct listnode *head = *head_ref;
    if (head == NULL || head->next == NULL) 
        return;

    struct listnode *middle = list_split(head);
    struct listnode *half = middle->next;
    middle->next = NULL;
    if (half != NULL) 
        half->prev = NULL;
    list_merge_sort(_list, &head);
    list_merge_sort(_list, &half);

    *head_ref = list_merge(_list, head, half);  
}
/*End*/

/**
 * @brief Tạo ra một struct list mới
 * 
 * @return struct list* nếu cấp phát thành công. NULL nếu cấp phát thất bại 
 */
struct list *list_new()
{
    struct list *new = (struct list *)malloc(sizeof(struct list));
    if(new == NULL){
        perror("Failed to create a new list\n");
        return NULL;
    }
    new->head = NULL;
    new->tail = NULL;
    new->cmp = NULL;
    new->del = NULL;
    new->count = 0;
    return new;
}


/**
 * @brief Tạo ra một struct list mới, gán giá trị 2 con trỏ hàm del và cmp
 * 
 * @param cmp_cb Con trỏ hàm vào hàm so sánh dữ liệu tự định nghĩa
 * @param del_cb Con trỏ hàm vào giải phóng dữ liệu tự định nghĩa
 * @return struct list* nếu cấp phát thành công. NULL nếu cấp phát thất bại 
 */
struct list *list_create(list_cmp_cb_t cmp_cb, list_del_cb_t del_cb){
    struct list *new_list = list_new(); // Gọi hàm list_new trả về con trỏ list mới nếu thành công
    if(new_list == NULL){
        return NULL;
    }
    new_list->del = del_cb;
    new_list->cmp = cmp_cb;
    return new_list;
}

/**
 * @brief Tạo ra một node mới
 * 
 * @param data con trỏ trỏ vào dữ liệu được trường data của struct node quản lý
 * @return struct listnode* nếu cấp phát thành công. NULL nếu thất bại
 */
struct listnode *create_node(void *data){
    struct listnode *new_node = (struct listnode *)malloc(sizeof(struct listnode));
    if (new_node == NULL)
    {
        return NULL;
    }
    new_node->next = NULL;
    new_node->prev = NULL;
    new_node->data = data;
    return new_node;
}

/**
 * @brief Tạo và thêm một node mới vào list (thêm vào cuối)
 * 
 * @param _list Địa chỉ struct list quản lý danh sách đang cần thêm node
 * @param data con trỏ trỏ đến dữ liệu để node mới quản lý
 * @return struct listnode* 
 */
struct listnode *listnode_add (struct list *_list, void *data){

    if (_list == NULL) // Nếu struct list truyền vào chưa được khởi tạo thì trả về NULL
    {
        return NULL; 
    }

    struct listnode *new_node = create_node(data); // Gọi hàm create_node để tạo một node mới
    if(new_node != NULL){
        if(_list->head == NULL){
            _list->head = new_node;
            _list->tail = new_node; 
        }else{
            _list->tail->next = new_node;
            new_node->prev = _list->tail;
            _list->tail = new_node;
        }
        _list->count++;
        return new_node;
    }
    return NULL;
}

struct listnode *listnode_sort_add (struct list *_list, void *data){
    if(_list == NULL)
        return NULL;

    if(data == NULL)
        return NULL;

    if (_list->cmp == NULL)
        return NULL;

    struct listnode *new_node = create_node(data);
    if(new_node != NULL){
        if(_list->head == NULL){
            _list->head = new_node;
            _list->tail = new_node;
            _list->count++;
            return new_node;
        }

        if(_list->cmp(new_node->data, _list->head->data) <= 0){
            new_node->next = _list->head;
            _list->head->prev = new_node;
            _list->head = new_node;
           // _list->head->prev = NULL;
            _list->count++;
            return new_node;
        }

        struct listnode *tmp;
        for (tmp = _list->head; tmp->next != NULL && _list->cmp(new_node->data, tmp->next->data) > 0; tmp = tmp->next);
        new_node->next = tmp->next;
        if(tmp->next == NULL)
            _list->tail = new_node;
        else
            tmp->next->prev = new_node;
        tmp->next = new_node;
        new_node->prev = tmp;
        _list->count++;
        return new_node;
    }
    return NULL;
}

/**
 * @brief Tìm node trong danh sách theo dữ liệu
 * 
 * @param _list Địa chỉ struct list quản lý danh sách
 * @param data Con trỏ đến dữ liệu cần tìm node chứa dữ liệu giống nó
 * @return struct listnode* của node cần tìm nếu có. NULL nếu không có node nào chứa dữ liệu cần tìm
 */
struct listnode *listnode_lookup (struct list *_list, void *data){
    
    if(_list->head == NULL) // Kiem tra list rong
        return NULL;

    struct listnode *node = _list->head;
    while (node != NULL){
        if(node->data != NULL){
            if(_list->cmp(node->data, data) == 0){
                return node;
            }
        }
        node = node->next;
    }
    return NULL;
}

/**
 * @brief Thêm một node vào danh sách trước một node mong muốn (nếu có)
 * 
 * @param _list Địa chỉ struct list quản lý danh sách đang cần thêm node
 * @param _node Địa chỉ node cần thêm một node khác vào trước nó 
 * @param data Con trỏ trỏ đến dữ liệu để node mới quản lý
 * @return struct listnode* nếu thêm node mới thành công. NULL nếu không tìm thấy node trong danh sách hoặc cấp phát thất bại
 */
struct listnode *listnode_add_before (struct list *_list, struct listnode *_node, void *data){
    if(_list == NULL)
        return NULL;

    if(_list->head == NULL) // Nếu danh sách rỗng thì trả về NULL
        return NULL;

    struct listnode *tmp = _list->head;
    while (tmp != NULL){
        if(tmp == _node){
            struct listnode *new_node = create_node(data);
            if(new_node != NULL){
                if (tmp == _list->head){
                    new_node->next = _list->head;
                    _list->head->prev = new_node;
                    _list->head = new_node;
                }else{
                    new_node->next = tmp;
                    tmp->prev->next = new_node;
                    new_node->prev = tmp->prev;
                    tmp->prev = new_node;
                }
                _list->count++;
                return new_node;
            }else
                return NULL;                
        }
        tmp = tmp->next;
    }
    return NULL;
}

/**
 * @brief Thêm một node vào danh sách sau một node mong muốn (nếu có)
 * 
 * @param _list Địa chỉ struct list quản lý danh sách đang cần thêm node
 * @param _node Địa chỉ node cần thêm một node khác vào trước nó 
 * @param data Con trỏ trỏ đến dữ liệu để node mới quản lý
 * @return struct listnode* nếu thêm node mới thành công. NULL nếu không tìm thấy node trong danh sách hoặc cấp phát thất bại
 */
struct listnode *listnode_add_after (struct list *_list, struct listnode *_node, void *data){
    if(_list == NULL)
        return NULL;

    if(_list->head == NULL) // Nếu danh sách rỗng thì trả về NULL
        return NULL;

    struct listnode *tmp = _list->head;
    while (tmp != NULL){
        if(tmp == _node){
            struct listnode *new_node = create_node(data);
            if(new_node != NULL){
                if(tmp == _list->tail){
                    _list->tail->next = new_node;
                    new_node->prev = _list->tail;
                    _list->tail = new_node;
                }else{
                    new_node->next = tmp->next;
                    tmp->next->prev = new_node;
                    tmp->next = new_node;
                    new_node->prev = tmp;
                }
                _list->count++;
                return new_node;
            }else
                return NULL;                
        }
        tmp = tmp->next;
    }
    return NULL;
}

/**
 * @brief Thêm một node mới và sắp xếp (dùng merge sort)
 * 
 * @param _list Địa chỉ struct list quản lý danh sách đang cần thêm node
 * @param data Con trỏ trỏ đến dữ liệu để node mới quản lý
 * @return struct listnode* cấp phát node mới và thêm thành công. NULL nếu cấp phát node mới thất bại
 */
struct listnode *listnode_add_sort (struct list *_list, void *data){
    if(_list == NULL)
        return NULL;

    struct listnode *new_node = listnode_add(_list, data); // Thêm node mới vào cuối list
    if(new_node != NULL){
        list_merge_sort(_list, &_list->head);
        _list->tail = _list->head;
        while (_list->tail->next != NULL)
        {
            _list->tail = _list->tail->next; // Cập nhật lại node tail sau khi sắp xếp
        }
        return new_node;
    }
    return NULL;
}

/**
 * @brief Thêm một node mới với dữ liệu chưa xuất hiện trong danh sách và sắp xếp
 * 
 * @param _list Địa chỉ struct list quản lý danh sách đang cần thêm node
 * @param data Con trỏ trỏ đến dữ liệu để node mới quản lý
 * @return 0 nếu node đã tồn tại hoặc thêm node mới thất bại. 1 nếu thêm node mới vào danh sách và sắp xếp thành công
 */
int listnode_add_sort_nodup (struct list *_list, void *data){
    if(_list == NULL)
        return 0;

    struct listnode *node = listnode_lookup(_list, data);
    if(node == NULL){
        node = listnode_add_sort(_list, data);
        if(node != NULL)
            return 1;
        else
            return 0;
    }
    return 0;
}

/**
 * @brief Xóa tất cả các node trong list và cả dữ liệu của nó.
 * 
 * @param _list Địa chỉ struct list quản lý danh sách
 */
void list_delete_all_node (struct list *_list){
    
    if(_list->head == NULL) // Kiem tra list rong
        return;

    struct listnode *head = _list->head;
    struct listnode *_node;
    while (head != NULL){
        _node = head->next;
        if(head->data != NULL)
            _list->del(head->data);
        free(head);
        head = _node;
    }
    _list->head = NULL;
    _list->tail = NULL;
    _list->count = 0;
}

/**
 * @brief Xóa danh sách: con trỏ list, node và dữ liệu của từng node
 * 
 * @param _list Địa chỉ struct list quản lý danh sách
 */
void list_free (struct list *_list)
{
    if(_list == NULL)
        return;
    list_delete_all_node(_list);
    free(_list);
}

/**
 * @brief Xóa node ở đầu danh sách
 * 
 * @param _list Địa chỉ struct list quản lý danh sách
 */
void listnode_delete_at_begin (struct list *_list){
    if(_list->head == NULL){
        return;
    }
    struct listnode *node = _list->head;
    _list->head = _list->head->next;
    if(_list->head != NULL){
        _list->head->prev = NULL;
    }else{
        _list->tail = NULL;
    }
    if(node->data != NULL)
        _list->del(node->data);
    free(node);
    _list->count--;
}

/**
 * @brief Xóa node ở cuối danh sách
 * 
 * @param _list Địa chỉ struct list quản lý danh sách
 */
void listnode_delete_at_end (struct list *_list){
    if(_list->head == NULL){
        return;
    }
    struct listnode *node = _list->tail;
    _list->tail = _list->tail->prev;
    if(_list->tail != NULL){
        _list->tail->next = NULL;
    }else{
        _list->head = NULL;
    }
    if(node->data != NULL)
        _list->del(node->data);
    free(node);
    _list->count--;
}

/**
 * @brief Xóa node trong danh sách theo dữ liệu
 * 
 * @param _list Địa chỉ struct list quản lý danh sách
 * @param data Con trỏ đến dữ liệu cần tìm node chứa dữ liệu giống nó để xóa
 */
void listnode_delete (struct list *_list, void *data)
{
    if(_list == NULL){
        return;
    }

    if(_list->head == NULL){
        return;
    }

    struct listnode *node = listnode_lookup (_list, data);
    if(node != NULL){
        if(node == _list->head)
            listnode_delete_at_begin(_list);
        else{
            if(node == _list->tail)
                listnode_delete_at_end(_list);
            else{
                node->prev->next = node->next;
                node->next->prev = node->prev;
                if(node->data != NULL)
                    _list->del(node->data);
                free(node);
                _list->count--;
            }
        }
    }else{
        return;
    }
}

/**
 * @brief Xóa node trong danh sách theo địa chỉ của node
 * 
 * @param _list Địa chỉ struct list quản lý danh sách
 * @param _node Địa chỉ của node cần xóa trong danh sách (nếu node có trong danh sách) 
 */
void list_delete_node (struct list *_list, struct listnode *_node){
    if(_list == NULL){
        return;
    }

    if(_list->head == NULL){
        return;
    }
fprintf(stdout, "Loi2\n");
    struct listnode *_ptr = _list->head;
    
    while (_ptr != NULL)
    {
        fprintf(stdout, "Loi2\n");
        if(_ptr == _node){
            if(_ptr == _list->head)
                listnode_delete_at_begin(_list);
            else{
                if(_ptr == _list->tail)
                    listnode_delete_at_end(_list);
                else{
                    _ptr->prev->next = _ptr->next;
                    _ptr->next->prev = _ptr->prev;
                    if(_ptr->data != NULL)
                        _list->del(_ptr->data);
                    free(_ptr);
                    _list->count--;
                }
            }
        return;
        }
        _ptr = _ptr->next;
    }
}

/**
 * @brief Tìm node có dữ liệu cần tìm và xóa dữ liệu của node đó 
 * 
 * @param _list Địa chỉ struct list quản lý danh sách
 * @param data Con trỏ đến dữ liệu mà cần tìm node chứa nó để xóa 
 */
void listnode_delete_data (struct list *_list, void *data){
    if(_list == NULL){
        return;
    }

    if(_list->head == NULL){
        return;
    }

    struct listnode *node = listnode_lookup (_list, data);
    if(node != NULL){ 
        _list->del(node->data);
        node->data = NULL;
    }else{
        return;
    }
}

/**
 * @brief Trả về con trỏ trỏ vào node đầu
 * 
 * @param _list 
 * @return con trỏ trỏ vào node đầu 
 */
void *listnode_head (struct list *_list){
    if(_list == NULL)
        return NULL;
    return _list->head;
}

