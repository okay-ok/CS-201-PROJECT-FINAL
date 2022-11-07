//**Vitesh Vitesh**
//  main.cpp
//  Dinic's Alogorithm with Link-Cut-Tree


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
class LinkCutTree {
    friend class LinkCutBlockFlowFinder;
private:
    std::vector <Node> nodes;
    
    Node* _cutout(Node* vertex);
    Node* _leftest(Node* vertex);
    Node* _expose(Node* vertex);
    Node* _cleanUp(Node* vertex);
    Node* _liftUpToRoot(Node* vertex);   //it's splay current vertex
    Node* _findLeftestMin(size_t minValue, Node* vertex);
public:
    Node* lastExposed;
    LinkCutTree(size_t _size);
    ~LinkCutTree();
    
    void clearTrees();
    
    void removeWeightInPath(size_t weight, size_t ind);
    void link(size_t indRoot, size_t indVert);
    void cut(size_t indVert, size_t indParent);
    void setWeight(size_t indVert, size_t weight);
    
    size_t getEdgeWeight(size_t indVert);
    Node* prevInPath(size_t ind);
    Node* getMinEdge(size_t ind);
    Node* findRoot(size_t ind);
};

//**********************************************************************************************
LinkCutTree::LinkCutTree(size_t sizeVert){
    nodes.resize(sizeVert, Node(0));
    for(size_t i = 0;i < nodes.size(); ++i) {
        new SplayTree(&nodes[i]);
        nodes[i].key = i;
    }
}

LinkCutTree::~LinkCutTree() {
    for(size_t i = 0;i < nodes.size(); ++i) {
        if(nodes[i].parent == nullptr) {
            SplayTree* buff = nodes[i].treePtr;
            //assert(buff->_root = &nodes[i]);
            buff->_root = nullptr;
            delete buff;
        }
    }
}

void LinkCutTree::clearTrees() {
    for(size_t i = 0;i < nodes.size(); ++i) {
        if(nodes[i].parent != nullptr) {
            nodes[i] = Node(i,0);
            nodes[i].treePtr = new SplayTree(&nodes[i]);
        } else {
            SplayTree* buff = nodes[i].treePtr;
            nodes[i] =  Node(i,0);
            nodes[i].treePtr = buff;
            //buff->_root = &nodes[i];
        }
    }
}

void LinkCutTree::link(size_t indRoot, size_t indVert) {
    Node* vertex = &nodes[indVert];
    Node* treeRoot = &nodes[indRoot];
    treeRoot->link = vertex;
    _expose(treeRoot);
}

void LinkCutTree::cut(size_t indVert, size_t indParent) {
    Node* vertex = &nodes[indVert];
    Node* parent = &nodes[indParent];
    _expose(parent);
    vertex->link = nullptr;
}

Node* LinkCutTree::findRoot(size_t ind) {
    Node* vertex = &nodes[ind];
    _expose(vertex);
    return _leftest(_liftUpToRoot(vertex));
}

Node* LinkCutTree::_cleanUp(Node* vertex) {
    Node* root;
    
    if(vertex->parent) {
        root = _cleanUp(vertex->parent);
    } else {
        root = vertex;
    }
    
    Node::push(vertex);
    
    return root;
}

inline Node* LinkCutTree::_liftUpToRoot(Node* vertex) {
    if(!vertex) {
        return nullptr;
    }
    
    if(!vertex->parent) {
        return vertex;
    }
    
    Node* root = _cleanUp(vertex);
    root->treePtr->splay(vertex);
    return vertex;
}

Node* LinkCutTree::_leftest(Node* root) {
    return root->treePtr->find(0);
}

Node* LinkCutTree::_cutout(Node* vertex) {
    _liftUpToRoot(vertex);
    std::pair<SplayTree*, SplayTree*> splitedTrees = SplayTree::split(vertex->treePtr, Node::getSize(vertex->leftChild) + 1);
    SplayTree* right = splitedTrees.second;
    if(right->getRoot()) {
        right->find(0)->link = vertex;
    } else {
        delete right;
    }
    return vertex;
}

Node* LinkCutTree::_expose(Node* vertex) {
    Node* next;
    vertex = _leftest(_liftUpToRoot(_cutout(vertex)));
    while(vertex->link != nullptr) {
        next = _cutout(vertex->link);
        vertex->link = nullptr;
        SplayTree::merge(_liftUpToRoot(next)->treePtr, _liftUpToRoot(vertex)->treePtr);
        vertex = _leftest(_liftUpToRoot(vertex));
    }
    return vertex;
}

Node* LinkCutTree::getMinEdge(size_t ind) {
    Node* vertex = &nodes[ind];
    _liftUpToRoot(vertex);
    size_t minValue = Node::getMin(vertex);
    return _findLeftestMin(minValue, vertex);
}

Node* LinkCutTree::_findLeftestMin(size_t minValue, Node* vertex) {
    Node::push(vertex);
    
    if(Node::getMin(vertex->leftChild) == minValue) {
        return _findLeftestMin(minValue, vertex->leftChild);
    }
    
    if(vertex->edgeWeight == minValue) {
        return vertex;
    }
    
    return _findLeftestMin(minValue, vertex->rightChild);
}

void LinkCutTree::setWeight(size_t indVert, size_t weight) {
    Node* vertex = &nodes[indVert];
    _liftUpToRoot(vertex);
    vertex->edgeWeight = weight;
    Node::updateNodeParams(vertex);
}

void LinkCutTree::removeWeightInPath(size_t added, size_t indVert) {
    Node::removeWeight(added, &nodes[indVert]);
}

size_t LinkCutTree::getEdgeWeight(size_t indVert) {
    Node* vertex = &nodes[indVert];
    _liftUpToRoot(vertex);
    Node::push(vertex);
    size_t edgeWeight = vertex->edgeWeight;
    return edgeWeight;
}

Node* LinkCutTree::prevInPath(size_t ind) {
    Node* source = &nodes[ind];
    _expose(findRoot(ind));
    return _leftest(_liftUpToRoot(source));
}

//************************************
//**Vitesh Vitesh**
