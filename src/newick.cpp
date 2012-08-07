/* 
 * File:   newick.cpp
 * Author: ask
 *
 * Created on June 20, 2012, 9:25 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <math.h>

#include <vector>
#include <string>
#include <map>

#define NAME_LEN  64
#define STRAIN_NUM 54
/*
 * 
 */

struct node;

typedef std::vector<std::string> clade_type;

struct node
{
	node* parent;
	std::vector<node*> childs;
	clade_type clade;
	int edge;
};

class tree
{
	std::map<std::string, clade_type> Topology;
	void add_subtree(FILE* f, node* root);
	void tre(node* r, FILE* f);
	void SetClades(node* root);
	void DeleteNullEdges(node* n);
	int count_nodes(node* n);
public:
	int node_num;
	int num;
	node* Root;
	tree(FILE* f);
	void Tree2Newick(const char* name);
	size_t treeOK();
	void RootCladeMinus(node* n);
	void FixClades(node* root);
};

void tree::DeleteNullEdges(node* n)
{
	for (int i = 0; i < n->childs.size(); ++i)
	{
		if (n->childs[i]->edge == 0 && n->childs[i]->childs.size() != 0)
		{
			node* child = n->childs[i];
			n->childs.erase(n->childs.begin()+i);
			for (int j = 0; j < child->childs.size(); ++j)
			{
				n->childs.push_back(child->childs[j]);
			}
			--i;
		}
	}
	for (int i = 0; i < n->childs.size(); ++i)
	{
		DeleteNullEdges(n->childs[i]);
	}
}

void tree::RootCladeMinus(node* n)
{
	if (n->clade.size() == STRAIN_NUM || n->clade.size() < STRAIN_NUM/2+STRAIN_NUM%2) return;
	int q = 0;
	std::vector<std::string> buf;
	for (int i = 0; i < Root->clade.size(); ++i)
	{
		q = 0;
		for (int j = 0; j < n->clade.size(); ++j)
		{
			if (Root->clade[i] == n->clade[j]) {q = 1; break;}
		}
		if (q == 0) buf.push_back(Root->clade[i]);
	}
	if (STRAIN_NUM%2 == 0)
	{
		if (n->clade.size() == STRAIN_NUM/2)
		{
			std::sort(buf.begin(), buf.end());
			std::sort(n->clade.begin(), n->clade.end());
			std::vector<std::string> _buf;
			_buf.push_back(buf[0]); _buf.push_back(n->clade[0]);
			std::sort(_buf.begin(), _buf.end());
			if (_buf[0] == buf[0]) {n->clade.swap(buf);}
	//		printf("<<<<<<<<<<< NCLADE\n");
	//		for (q = 0; q < n->clade.size(); ++q)
			{
	//			printf("\t%s\n", n->clade[q].c_str());
			}
		}
		else n->clade.swap(buf);
	}
}

void tree::FixClades(node* n)
{
//	printf("FIX CLADE SIZE %lu\n", n->clade.size());
	if (n->clade.size() < STRAIN_NUM/2+STRAIN_NUM%2) return;
	if (n->clade.size() != STRAIN_NUM) RootCladeMinus(n);
	for (int i = 0; i < n->childs.size(); ++i)
	{
		FixClades(n->childs[i]);
	}
}

size_t tree::treeOK()
{
	if (Root != NULL) return Root->childs.size();
	return 666;
}

void tree::SetClades(node* root)
{
	if (root->childs.size() == 0) return;
	for (int i = 0; i < root->childs.size(); ++i)
	{
		SetClades(root->childs[i]);
		for (int j = 0; j < root->childs[i]->clade.size(); ++j)
		{
			root->clade.push_back(root->childs[i]->clade[j]);
		}
	}
	return;
}

void tree::tre(node* r, FILE* f)
{
	int i;
	fprintf(f, "(");
	for (i = 0; i < r->childs.size(); ++i)
	{
		if (r->childs[i]->childs.size() == 0)
		{
			fprintf(f, "%s", r->childs[i]->clade[0].c_str());
			if (r->childs[i]->edge != 0)
				fprintf(f, ":%d", r->childs[i]->edge);
		}
		else
		{
			tre(r->childs[i], f);
		}
		if (i + 1 < r->childs.size()) fprintf(f, ",");
	}
	fprintf(f, ")");
	if (r != Root && r->edge != 0)
	{
		printf(":%d", r->edge);
	}
}

void tree::Tree2Newick(const char* name)
{
	printf("TREE OUTPUT\n");
	FILE* f = stdout;//fopen(name, "w+");
	if (!f)
	{
		printf("Err: output stream ?\n");
		exit(1);
	}
	tre(Root, f);
	fprintf(f, ";\n");
}


void tree::add_subtree(FILE* f, node* root)
{
	char c = fgetc(f);
	int l = 0; int r = 0;
	char str[128];
	int p = 0;
	int is_leaf = 0;
	int i_edge = 0;
	while (1)
	{
		char edge[10]; strcpy(edge, "");
		if (c == ',')
		{
			str[p] = '\0';
			if (is_leaf == 1 || p > 0)
			{
				node* n = new node;
				n->parent = root;
				n->edge = i_edge;
				i_edge = 0;
				std::string strname = str;
				n->clade.push_back(str);
				root->childs.push_back(n);
			}
			else
			{
				root->childs.back()->edge = i_edge;
				i_edge = 0;
			}
			p = 0;
		}
		else if (c == '(')
		{
			is_leaf = 0; p = 0; strcpy(str, "");
			node* n = new node;
			n->parent = root;
			root->childs.push_back(n);
			add_subtree(f, n);
		}
		else if (c == ')')
		{
			str[p] = '\0';
			if (is_leaf == 1 || p > 0)
			{				
				node* n = new node;
				n->parent = root;
				n->edge = i_edge;
				i_edge = 0;
				std::string strname = str;
				n->clade.push_back(str);
				root->childs.push_back(n);
			}
			else
			{
				root->childs.back()->edge = i_edge;
				i_edge = 0;
			}
			return;
		}
		else if (c != ' ' && c != '\n' && c != ':')
		{
			is_leaf = 1;
			str[p++] = c;
		}
		if (c == ':')
		{
			int e = 0;
			while(c != ')' && c != ',' && c != ';')
			{				
				c = fgetc(f);
				if (c != ')' && c != ',' && c != ';') edge[e++] = c;
			}
			edge[e] = '\0';
			i_edge = atoi(edge);
		}
		else c = fgetc(f);
	}
}

int tree::count_nodes(node* n)
{
	for (int i = 0; i < n->childs.size(); ++i)
	{
		++node_num;
		count_nodes(n->childs[i]);
	}
	return 1;
}

tree::tree(FILE* f)
{
	node_num = 0;
	Root = new node;
	if (!f) {printf("Err: can't read from file\n"); exit(1);}
	char c = fgetc(f);
	while (c != '>' && !feof(f)) c = fgetc(f);
	if (c == '>')
	{
		fscanf(f,"%d", &num);
	}
	else
	{
		return;
	}
	while (c != '(' && !feof(f)) {c = fgetc(f);}
	Root->childs.erase(Root->childs.begin(), Root->childs.end());
	if (!feof(f)) add_subtree(f, Root);
	else {printf("Last tree readed\n"); return;}
	c = fgetc(f);
	if (c == ';') 
	{
//		printf("SUCCESS; %lu\n", Root->childs.size());
	}
	else {printf("ERR: ; ?\n"), exit(1);}

	printf("OK\n");
	DeleteNullEdges(Root);
	SetClades(Root);
	FixClades(Root);
//	printf("OK1\n");

	if (Root->childs.size() == 2)
	{
		if (Root->childs[0]->edge > 0 && Root->childs[1]->edge > 0)
		{
			if (Root->childs[0]->childs.size() > 0)
			{
				for (int i = 0; i < Root->childs[0]->childs.size(); ++i)
				{
					Root->childs[0]->childs[i]->parent = Root;
					Root->childs.push_back(Root->childs[0]->childs[i]);
				}
				Root->childs.erase(Root->childs.begin());
			}
			else if (Root->childs[1]->childs.size() > 0)
			{
				node n = *Root->childs[1];
				Root->childs.erase(Root->childs.begin()+1);
				for (int i = 0; i < Root->childs[1]->childs.size(); ++i)
				{
					n.childs[i]->parent = Root;	
					Root->childs.push_back(n.childs[i]);					
				}
			}
		}
	}
	count_nodes(Root);
//	printf("Root childs size %lu\n", Root->childs.size());
	Tree2Newick("test");
//	int i;
//	for (i = 0; i < Root->childs.size(); ++i)
//	{
//		printf("%lu;\n", Root->childs[i]->clade.size());
//	}
}


int FindClade(tree T, node* x, node* y, int* q);
int CompareClades(tree T, node* x, node* y);

int CompareTopology(tree T, node* x, node* y, int* score, int* _D)
{
//	printf("D = %d\n", (*_D)++);
	if (y->childs.size() == 0) { return *score; }
//	printf("Y clade size = %lu\n", y->clade.size());
	int q = 0;
	FindClade(T, x, y, &q);
	if (q == 0) 
	{
		*score += 1; 
//		printf("\t>>>>>>>>>> Y CLADE SIZE = %lu; score = %d\n", y->clade.size(), *score);
//		if (y->clade.size()>27)
		{
//			printf("ALARM!\n");
			std::sort(y->clade.begin(), y->clade.end());
//			for (int s = 0; s < y->clade.size(); ++s)
			{
//					printf("\t%s\n", y->clade[s].c_str());
			}
		}
	}
	for (int i = 0; i < y->childs.size(); ++i)
	{
//		printf("CompareTopology %d %lu\n", i, y->childs[i]->childs.size());
		CompareTopology(T, x, y->childs[i], score, _D);
	}
	return *score;
}

int FindClade(tree T, node* x, node* y, int* q)
{
//	printf("FINDCLADE q = %d\n", *q);
	if (*q == 1) {return 0;}
	if (CompareClades(T, x, y) == 0) { *q = 1; return 0;}
	if (x->childs.size() == 0)
	{
//		if (x->clade[0] == y->clade[0]) *q = 1;
//		printf("NULL clade size = %lu\n", x->clade.size());
		return 0;
	}	
	for (int i = 0; i < x->childs.size(); ++i)
	{
//		printf("FINDCLADE %d\n", i);
		FindClade(T, x->childs[i], y, q);
	}
}

int CompareClades(tree T, node* x, node* y)
{
//	printf("COMPARE\n");
	int q = 0;
	if (x->clade.size() < y->clade.size()) {
		//printf("LESS\n");
		return 1;}
	else if (x->clade.size() == y->clade.size())
	{
//		printf("EQUAL %lu\n", x->clade.size());
		for (int i = 0; i < y->clade.size(); ++i)
		{
			q = 0;
			for (int j = 0; j < x->clade.size(); ++j)
			{
				if (y->clade[i] == x->clade[j]) q = 1;
			}
			if (q == 0) {return 1;}
		}
		return 0;
	}
	else// if ( 1 == 0)
	{
		int N = (int)x->childs.size();
		int k;
		int* A = (int*)calloc(2, sizeof(int));
		node* n = new node;
		for (k = 2; k < N; ++k)
		{
			A = (int*)realloc(A, k*sizeof(int));
			for (int i = 0; i < k; ++i)
			{
				A[i] = i;
			}
			int p = k;
			int C = 0;
			while (p > -1)
			{
				n->clade.erase(n->clade.begin(), n->clade.end());
				for (int i = 0; i < k; ++i)
				{
					for (int s = 0; s < x->childs[A[i]]->clade.size(); ++s)
					{
						int L = 0;
						for (int v = 0; v < n->clade.size(); ++v)
						{
							if (n->clade[v] == x->childs[A[i]]->clade[s])
							{
								L = 1;
							}
						}
						if (L == 0) n->clade.push_back(x->childs[A[i]]->clade[s]);
					}
//					n->clade.insert(n->clade.end(), x->childs[A[i]]->clade.begin(), x->childs[A[i]]->clade.end());
				}
				if (n->clade.size() == y->clade.size())
				{
//					printf("A k = %d; N = %d: ", k,N);
//					for (int s = 0; s < k; ++s)
					{
//						printf("%2d ", A[s]);
					}
//					printf(";\n");
					for (int s = 0; s < n->clade.size(); ++s)
					{
//						printf("\tn: %3s; y: %3s\n", n->clade[s].c_str(), y->clade[s].c_str());
					}
//					printf("=============\n");
					T.FixClades(n);
					if (CompareClades(T, n, y) == 0) { 
//							printf("FUCK YEAH\n");
							return 0;}
				}
				C++;
				if (A[k-1] == N-1) --p;
				else p = k-1;
				if (p > -1)
				{
					for (int i = k-1; i > p-1; --i)
					{
						A[i] = A[p]+i-p+1;
					}
				}
			}
//			printf("k = %d; N = %d; C = %d\n", k, N, C);
		}
	}
	return 1;
}

bool CompareTrees(tree* a, tree* b)
{
	return (a->num < b->num);
}

int main(int argc, char** argv)
{
	if (argc != 2){printf("Usage: progname <filename>\n"); exit(1);}
	FILE* f = fopen(argv[1], "r");
	if (! f){printf("Err: can't open file %s\n", argv[1]); exit(1);}

	std::vector<tree*> trees;
	int i = 0;
	//tree* T;
	while (!feof(f))
	{
		tree* T = new tree(f);
		if (T->Root->childs.size() > 0){
		trees.push_back(T); ++i;}
//		printf("TREE %d LOADED\n", ++i);}
//		if (i == 500) break;
	}
	
	std::sort(trees.begin(), trees.end(), CompareTrees);
	
	int score = 0;
	int** res = (int**)calloc(trees.size(), sizeof(int*));
	for (i = 0; i < trees.size(); ++i)
	{
		res[i] = (int*)calloc(trees.size(), sizeof(int));
	}

	FILE* f_out = fopen("Q4_1500_1628.txt", "a+");
	int D = 0;
//	if (1 == 0)
	{
//		for (i = 0; i < trees.size(); ++i)
		for (i = 1500; i < 1628; ++i)
		{
			fprintf(f_out, "[%d] ", i);
			printf(" i = %d\n",i);
			if (trees[i]->node_num > 80)
			{
				for (int j = 0; j < trees.size(); ++j)
				{
					score = 0;
					printf("::::::::::: i=%d; j=%d;\n", i,j);
					if (i != j)
					{
						res[i][j] = CompareTopology(*trees[i], trees[i]->Root, trees[j]->Root, &score, &D);
					}
					else res[i][j] = 0;
					fprintf(f_out, "%d ", res[i][j]);
				}
			}
			else fprintf(f_out, "TOOPOLY");
			fprintf(f_out, "\n");
		}
	}
	
//	for (int i = 0; i < trees.size(); ++i)
	{
//		printf("%d\n", trees[i]->node_num);
	}
	
	score = 0;
//	printf("RESULT %d\n", CompareTopology(*trees[1], trees[1]->Root, trees[0]->Root, &score));
	return (EXIT_SUCCESS);
}

