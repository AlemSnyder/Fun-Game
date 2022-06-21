#ifndef __NODE_HPP__
#define __NODE_HPP__

template<class T>
class Node {  // Used to find paths.
public:
    Node(T *tile, float hc);
    Node();// needs to stay but should not be used
    void init(T *tile, float hc);

    inline bool is_open() { return !this->tile->is_solid(); }
    void explore(Node<T> *parent, float gc);
    void explore();  //

    inline bool operator<(const Node<T> *other) {
        return this->fCost < other->fCost;
    }
    inline bool operator>(const Node<T> *other) {
        return this->fCost > other->fCost;
    }
    // Setters

    // Getters
    T *get_tile() { return tile; }
    const T *get_tile() const { return tile; }
    Node<T> *get_parent() { return parent_node; }

    float get_gCost() const { return gCost; } //Time from start to this node
    float get_hCost() const { return hCost; } //Minimum time from this node to end
    float get_fCost() const { return fCost; } //Minimum time from start to end through this node
    bool is_explored() const { return explored; }

private:
    T *tile;
    Node *parent_node;

    float gCost; //Time from start to this node
    float hCost; //Minimum time from this node to end
    float fCost; //Minimum time from start to end through this node
    bool explored;
};

template<class T>
void Node<T>::init(T *tile_, float hc) {
    parent_node = NULL;
    tile = tile_;
    gCost = 0;
    hCost = hc;
    fCost = gCost + hCost;
    explored = false;
}

template<class T>
Node<T>::Node(T *tile_, float hc) { init(tile_, hc); }

template<class T>
Node<T>::Node() {
    parent_node = NULL;
    tile = nullptr;
    gCost = 0;
    hCost = 0;
    fCost = gCost + hCost;
    explored = false;
}

template<class T>
void Node<T>::explore(Node<T> *parent, float gc) {
    if (explored){
        if (gCost > gc){
            this->parent_node = parent;
            gCost = gc;
            fCost = gCost + hCost;
        }
    } else{
        this->parent_node = parent;
        gCost = gc;
        fCost = gCost + hCost;
        explored = true;
    }
}

template<class T>
void Node<T>::explore() {  // explore for start
    this->parent_node = NULL;
    gCost = 0;  // distance from start should be 0 when this is the start
    hCost = 0;
    fCost = gCost + hCost;
    explored = true;
}

#endif