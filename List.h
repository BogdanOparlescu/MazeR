#include<iostream>

template <typename T>
class List
{
public:
    template <typename t>
    class Node
    {
    public:
        t value;
        Node *next;

        Node(t value) : value(value), next(NULL) {}
    };

    Node<T> *head;

//public:
    List() : head(NULL) {}

    int empty()
    {
        return (head == NULL);
    }

    int length()
    {
        int k=0;
        for(Node<T>* n=head;n;n=n->next)
            k++;
        return k;
    }

    void prepend(T element)
    {
        Node<T> *n = new Node<T>(element);
        n->next = head;
        head = n;
    }
    
    void append(T element)
    {
        Node<T> *n = new Node<T>(element);

        if (empty())
        {
            head = n;
            return;
        }

        Node<T> *c = head;
        while (c->next)
            c = c->next;
        c->next = n;
    }

    void remove(T element)
    {
        List<T> new_list=List<T>();
        for(Node<T>* n=head;n;n=n->next)
            if(n->value!=element)
                new_list.append(n->value);
        head=new_list.head;
    }

    int contains(T element)
    {
        for(Node<T>* n=head;n;n=n->next)
            if(n->value==element)
                return 1;
        return 0;
    }

    friend std::ostream &operator<<(std::ostream &os, List<T> list)
    {
        for (Node<T> *c = list.head; c; c = c->next)
            os << c->value << ' ';
        os << '\n';
    }
};