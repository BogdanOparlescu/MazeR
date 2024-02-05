template <typename T>
class Stack
{
    template <typename t>
    class Node
    {
    public:
        t value;
        Node *next;

        Node(t value) : value(value), next(NULL) {}
    };

    Node<T> *head;

public:
    Stack() : head(NULL) {}

    int empty()
    {
        return (head == NULL);
    }

    void push(T element)
    {
        Node<T> *n = new Node<T>(element);
        n->next = head;
        head = n;
    }

    T peek()
    {
        return (head->value);
    }

    T pop()
    {
        T _peek = peek();
        head = head->next;
        return _peek;
    }
};