

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <algorithm>
#include <cassert>
#define nullptr NULL


//#define SIZE_T_MAX 1e15
using namespace std;

//**********************************************************************************************

class Node;
class SplayTree;

const size_t INF = 1e16;

class Node{
public:
    static void removeWeight(size_t value, Node* vertex);
    static void updateNodeParams(Node* vertex);
    static void recursiveDelete(Node* vertex);
    static void push(Node* vertex);
    
    static size_t getSize(Node* vertex);
    static size_t getMin(Node* vertex);
    static size_t getSum(Node* vertex);
    
    Node(size_t key, size_t weight = 0);
    
    size_t key;
    size_t edgeWeight;
    size_t sizeOfSubtree;
    size_t subtreeMinWeight;
    size_t removedWeightValue;
    
    Node* leftChild;
    Node* rightChild;
    Node* parent;
    Node* link;
    
    SplayTree* treePtr;
};

class SplayTree
{
    friend class LinkCutTree;
private:
    Node* _find(size_t position, Node* vertex);
    void _keepParent(Node* vertex);
    void _setParent(Node* parent, Node* vertex);
    void _rotate(Node* parent, Node* vertex);
    void _merge(SplayTree* addedTree); //added tree is right merged tree
    SplayTree* _split(size_t position); //returned tree is tight splited tree
    
    Node* _root;
public:
    SplayTree(Node* root);
    ~SplayTree();
    
    Node* find(size_t position);
    
    static SplayTree* merge(SplayTree* leftTree, SplayTree* rightTree);
    static std::pair<SplayTree*, SplayTree*> split(SplayTree* tree,size_t position);
    
    void splay(Node* vertex);
    
    Node* getRoot() { return _root; };
};


//**********************************************************************************************



Node::Node(size_t key, size_t edgeWeight):key(key), sizeOfSubtree(1), leftChild(nullptr), rightChild(nullptr), parent(nullptr), link(nullptr), subtreeMinWeight(edgeWeight), removedWeightValue(0), treePtr(nullptr), edgeWeight(edgeWeight) {
}

void Node::recursiveDelete(Node* vertex) {
    if(vertex) {
        Node::recursiveDelete(vertex->leftChild);
        Node::recursiveDelete(vertex->rightChild);
        delete vertex;
    }
}

void Node::removeWeight(size_t value, Node* vertex) {
    if(vertex) {
        vertex->removedWeightValue += value;
    }
}

void Node::push(Node* vertex) {
    if(vertex) {
        vertex->edgeWeight -= vertex->removedWeightValue;
        Node::removeWeight(vertex->removedWeightValue, vertex->leftChild);
        Node::removeWeight(vertex->removedWeightValue, vertex->rightChild);
        vertex->removedWeightValue = 0;
        Node::updateNodeParams(vertex);
    }
}

SplayTree::SplayTree(Node* root) {
    _root = root;
    if(root) {
        root->treePtr = this;
    }
}

SplayTree::~SplayTree() {
    Node::recursiveDelete(_root);
}

void SplayTree::_setParent(Node* vertex, Node* parent) {
    if(vertex) {
        vertex->parent = parent;
    }
}

size_t Node::getSize(Node* vertex) {
    if(vertex) {
        return vertex->sizeOfSubtree;
    }
    return 0;
}

size_t Node::getMin(Node* vertex) {
    if(vertex) {
        return vertex->subtreeMinWeight - vertex->removedWeightValue;
    }
    return INF;
}

void Node::updateNodeParams(Node* vertex) {
    if(vertex) {
        vertex->sizeOfSubtree = getSize(vertex->leftChild) + getSize(vertex->rightChild) + 1;
        vertex->subtreeMinWeight = min(min(getMin(vertex->leftChild), getMin(vertex->rightChild)), vertex->edgeWeight);
    }
}

void SplayTree::_keepParent(Node* vertex) {
    _setParent(vertex->leftChild, vertex);
    _setParent(vertex->rightChild, vertex);
    Node::updateNodeParams(vertex);
}

void SplayTree::_rotate(Node* parent, Node* vertex) {
    Node* grandParent = parent->parent;
    
    Node::push(grandParent);
    Node::push(parent);
    Node::push(vertex);
    
    if(grandParent) {
        if(grandParent->leftChild == parent) {
            grandParent->leftChild = vertex;
        } else {
            grandParent->rightChild = vertex;
        }
    }
    
    if(parent->leftChild == vertex) {
        parent->leftChild = vertex->rightChild;
        vertex->rightChild = parent;
    } else {
        parent->rightChild = vertex->leftChild;
        vertex->leftChild = parent;
    }
    
    _keepParent(parent);
    _keepParent(vertex);
    
    _setParent (vertex, grandParent);
}

void SplayTree::splay(Node* vertex){
    while(true) {
        if(!vertex->parent) {
            _root = vertex;
            _root->treePtr = this;
            return;
        }
        
        Node* parent = vertex->parent;
        Node* grandParent = parent->parent;
        
        if(!grandParent) {
            _rotate(parent, vertex);
            _root = vertex;
            _root->treePtr = this;
            return;
        }
        
        bool zigZigFlag = ((grandParent->leftChild == parent) == (parent->leftChild == vertex));
        
        if(zigZigFlag) {
            _rotate(grandParent, parent);
            _rotate(parent, vertex);
        } else {
            _rotate(parent, vertex);
            _rotate(grandParent, vertex);
        }
    }
}

Node* SplayTree::find(size_t position) {
    size_t treeSize = Node::getSize(_root);
    
    if(position >= treeSize) {
        return NULL;
        // throw std::out_of_range("out of range in SplayTree::find\n");
    }
    
    return _find(position, _root);
}

Node* SplayTree::_find(size_t position, Node* vertex) {
    Node::push(vertex);
    
    size_t indexLeft = Node::getSize(vertex->leftChild);
    
    if(position == indexLeft) {
        splay(vertex);
        return vertex;
    }
    
    if(position < indexLeft) {
        return _find(position, vertex->leftChild);
    }
    
    return _find(position - indexLeft - 1, vertex->rightChild);
}

std::pair<SplayTree*, SplayTree*> SplayTree::split(SplayTree* tree,size_t position) {
    SplayTree* leftTree = nullptr;
    SplayTree* rightTree = nullptr;
    if(tree) {
        rightTree = tree->_split(position);
        leftTree = tree;
    }
    return std::make_pair(leftTree, rightTree);
}


SplayTree* SplayTree::_split(size_t position){
    size_t treeSize = (_root ? _root->sizeOfSubtree : 0);
    
    if(position > treeSize) {
        return NULL;
        //    throw std::out_of_range("out of range in SplayTree::split\n");
    }
    
    if(position == treeSize) {
        return new SplayTree(nullptr);
    }
    
    Node* newRoot = _find(position, _root);
    
    SplayTree* rightTree = new SplayTree(newRoot);
    
    _root = newRoot->leftChild;
    newRoot->leftChild = nullptr;
    _setParent(_root, nullptr);
    
    if(rightTree->_root) {
        rightTree->_root->treePtr = rightTree;
    }
    
    Node::push(rightTree->_root);
    Node::push(_root);
    
    return rightTree;
}

SplayTree* SplayTree::merge(SplayTree* leftTree, SplayTree* rightTree) {
    if(!leftTree) {
        return rightTree;
    }
    
    leftTree->_merge(rightTree);
    
    return leftTree;
}

void SplayTree::_merge(SplayTree* addedTree) {
    if(!addedTree->_root)
    {
        delete addedTree;
        addedTree = nullptr;
    }
    
    
    if(!_root) {
        _root = addedTree->_root;
        addedTree->_root = nullptr;
        delete addedTree;
        addedTree = nullptr;
        return;
    }
    
    find(_root->sizeOfSubtree - 1);
    addedTree->find(0);
    
    Node::push(_root);
    
    _root->rightChild = addedTree->_root;
    addedTree->_root = nullptr;
    delete addedTree;
    addedTree = nullptr;
    _keepParent(_root);
}

//**********************************************************************************************