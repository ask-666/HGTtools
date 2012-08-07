/* 
 * File:   main.cpp
 * Author: ask
 *
 * Created on May 26, 2012, 2:03 PM
 */

#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <algorithm>


#define STRLEN 1024

struct SNPset;
struct BlockSet;

typedef std::vector<SNPset*> SNPset_container;
typedef std::vector<BlockSet*> BlockSet_container;

struct singleSNP
{
	std::string str;
	std::vector<int> pos;
	SNPset_container SNPsets;
};

struct SNPset
{
	char C;
	std::vector<int> set;
};

struct BlockSet
{
	std::vector<int> set;
};

struct Block
{
	BlockSet_container BlockSets;
	std::vector<int> begin;
	std::vector<int> end;
};

bool singleSNPcompare(singleSNP*, singleSNP*);

class allSNPs
{
	typedef std::vector<singleSNP*> SNPs_container;
	typedef std::vector<Block*> Block_container;
	
	SNPs_container SNPs;
	Block_container Blocks;
	
	void setSNPset(singleSNP* snp);
	bool isSNPinBlock(singleSNP* snp, Block* block);
	void setBeginBlock(singleSNP* snp, Block* block);
	void setEndBlock(singleSNP* snp, Block* block);
	int CompareSets(SNPset_container& s, BlockSet_container& b);
public:
	allSNPs(const char* fname);
	void addSNP(singleSNP* snp);
	void outSNPs();
	void sortSNPs();
	void CreateBlocks();
	void outBlocks();
};

int is_in_set(int c, std::vector<int> vec)
{
	for (int i = 0; i < vec.size(); ++i)
	{
		if (c == vec[i]) return 1;
	}
	return 0;
}

void allSNPs::outBlocks()
{
	printf("Blocks number %lu\n\n", Blocks.size());
	Block_container::const_iterator it;
	int n = 0;
	for (it = Blocks.begin(); it != Blocks.end(); ++it)
	{
		printf("Block %d\n", n++);
		printf("Begin %d; End %d\n", (*it)->begin.front(), (*it)->end.front());
		for (int i = 0; i < (*it)->BlockSets.size(); ++i)
		{
			printf("\tset_%d ", i+1);
			for (int j = 0; j < (*it)->BlockSets[i]->set.size(); ++j)
			{
				printf("%d ", (*it)->BlockSets[i]->set[j]);
			}
			printf("\n");
		}
	}
}

int allSNPs::CompareSets(SNPset_container& s, BlockSet_container& b)
{
	for (int i = 0; i < b.size(); ++i)
	{
		std::sort(b[i]->set.begin(), b[i]->set.end());
	}
	for (int i = 0; i < s.size(); ++i)
	{
		std::sort(s[i]->set.begin(), s[i]->set.end());
	}
	for (int i = 0; i < s.size(); ++i)
	{
		int set_n = -1;
		for (int j = 0; j < b.size(); ++j)
		{
			if (is_in_set(s[i]->set[0], b[j]->set) == 1) //found
			{
				set_n = j; break;
			}
		}
		if (set_n > -1) 
		{
			for (int j = 1; j < s[i]->set.size(); ++j)
			{
				if (is_in_set(s[i]->set[j], b[set_n]->set) == 0) return 0; //not found
			}
		}
		else return 0;
	}
	return 1;
}

bool allSNPs::isSNPinBlock(singleSNP* snp, Block* block)
{
	if (block->begin.size() == 0) return true;
	
	if (CompareSets(snp->SNPsets, block->BlockSets) == 0) return false;
	return true;
}

void allSNPs::setBeginBlock(singleSNP* snp, Block* block)
{
	printf("BLOCK BEGIN:\n");
	for (int i = 0; i < snp->SNPsets.size(); ++i)
	{
		printf("set_%i: ", i);
		BlockSet* block_set = new BlockSet;
		for (int j = 0; j < snp->SNPsets[i]->set.size(); ++j)
		{
			printf("%d ", snp->SNPsets[i]->set[j]);
			block_set->set.push_back(snp->SNPsets[i]->set[j]);
		}
		printf("\n");
		block->BlockSets.push_back(block_set);
	}
	for (int i = 0; i < snp->pos.size(); ++i)
	{
		block->begin.push_back(snp->pos[i]);
	}
	return;	
}

void allSNPs::setEndBlock(singleSNP* snp, Block* block)
{
	printf("BLOCK END: ");
	for (int i = 0; i < snp->pos.size(); ++i)
	{
		printf("%d ", snp->pos[i]);
		block->end.push_back(snp->pos[i]);
	}
	printf("\n");
	return;
}

void allSNPs::CreateBlocks()
{
	printf("\nCreate Blocks\n");
	SNPs_container::const_iterator it;
	Block* block = new Block;
	for (it = SNPs.begin(); it != SNPs.end(); ++it)
	{
		printf("%s\t", (*it)->str.c_str());
		if (isSNPinBlock(*it, block))
		{
			printf("In Block\n");
			if (block->begin.size() == 0) setBeginBlock(*it, block);
		}
		else
		{
			printf("Not in Block\n");
			setEndBlock(*(it-1), block);
			Blocks.push_back(block);
			block = new Block;
			setBeginBlock(*it, block);
		}
		if (it+1 == SNPs.end())
		{
			setEndBlock(*it, block);
			Blocks.push_back(block);
		}	
	}
	
}

bool singleSNPcompare(singleSNP* a, singleSNP* b)
{
	return (a->pos.front() < b->pos.front());
}

allSNPs::allSNPs(const char* fname)
{
	FILE* f = fopen(fname, "r");
	
	if (!f)
	{
		printf("Error: can't open file %s\n", fname);
		return;
	}
	
	char* s = (char*)calloc(STRLEN, sizeof(char));
	fgets(s, STRLEN, f);
	
	while (!feof(f))
	{
		s = fgets(s, STRLEN, f);
		if (s)
		{
			if (strchr(s, '-') == NULL)
			{
				singleSNP* snp = new singleSNP;
				char* c = strtok(s, "\t\n ");
				if (c != NULL)
				{
					snp->str = c;
					setSNPset(snp);
					while (c != NULL)
					{
						c = strtok(NULL, "\t\n ");
						if (c)
						{
							int pos = atoi(c);
							snp->pos.push_back(pos);
						}
					}
					addSNP(snp);
				}
			}
		}
	}
	fclose(f);
}

void allSNPs::setSNPset(singleSNP* snp)
{
	std::string s = snp->str;
	SNPset* set = new SNPset;
	set->C = s[0];
	set->set.push_back(0);
	snp->SNPsets.push_back(set);
	for (int i = 1; i < s.size(); ++i)
	{
		int _set_is_found = 0;
		for (int j = 0; j < snp->SNPsets.size(); ++j)
		{
			if (snp->SNPsets[j]->C == s[i])
			{
				snp->SNPsets[j]->set.push_back(i);
				_set_is_found = 1;
				break;
			}
		}
		if (_set_is_found == 0)
		{
			SNPset* _set = new SNPset;
			_set->C = s[i];
			_set->set.push_back(i);
			snp->SNPsets.push_back(_set);
		}
	}
	for (int i = 0; i < snp->SNPsets.size(); ++i)
	{
		std::sort(snp->SNPsets[i]->set.begin(), snp->SNPsets[i]->set.end());
	}
}

void allSNPs::addSNP(singleSNP* snp)
{
	SNPs.push_back(snp);
	return;
}

void allSNPs::outSNPs()
{
	for (int i = 0; i < SNPs.size(); ++i)
	{
		const singleSNP* snp = SNPs[i]; 
		printf("%s ", snp->str.c_str());
		for (int j = 0; j < snp->pos.size(); ++j)
		{
			int pos = snp->pos[j];
			printf("%d ", pos);
		}
		printf("\n");
	}
	return;
}

void allSNPs::sortSNPs()
{
	std::sort(SNPs.begin(), SNPs.end(), singleSNPcompare);
}

int main(int argc, char** argv)
{
	if (argc < 2) 
	{
		printf("Usage: prog <filename>\n");
		return 1;
	}
	
	allSNPs S(argv[1]);
	S.sortSNPs();
//	printf("\n");
//	S.outSNPs();
	S.CreateBlocks();
	S.outBlocks();
	
	return 0;
}

