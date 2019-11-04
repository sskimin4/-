#include "tetris.h"

static struct sigaction act, oact;
/*
int limit=0;
int command;
nptr lastnode=NULL;
nptr ranking=NULL;
int rank_num=0;
*/
int main(){
	int exit=0;
	int i,j;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	
	
		
	srand((unsigned int)time(NULL));
	
	recRoot=(RecNode *)malloc(sizeof(RecNode));
	recRoot->lv=-1;
	recRoot->score=0;
	recRoot->recField=field;
	/*
	for(i=0;i<HEIGHT;i++)
		for(j=0;j<WIDTH;j++)
			recRoot->recField[i][j]=field[i][j];
	*/
	makeTree(recRoot);	


	while(!exit){
		createRankList();
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_REC_PLAY: recommendedPlay(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;
	for(i=0;i<VISIBLE_BLOCKS;i++)
		nextBlock[i]=rand()%7;
	//makeTree(recRoot);
	recommend(recRoot);
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;
	
	
	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	for(i=2;i<BLOCK_NUM;i++)
	{
		move(2+(i-1)*7,WIDTH+10);
		DrawBox(3+(i-1)*7,WIDTH+10,4,8);
	}

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(9+(BLOCK_NUM-2)*7,WIDTH+10);
	printw("SCORE");
	DrawBox(10+(BLOCK_NUM-2)*7,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		while(1)
		{
			if(CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)==0) break;
			blockY++;
			DrawChange(field,KEY_DOWN,nextBlock[0],blockRotate,blockY,blockX);	
		}
		//BlockDown(-1);
		//DrawChange(field,KEY_DOWN,nextBlock[0],blockRotate,blockY,blockX);
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(11+(BLOCK_NUM-2)*7,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j, k;
	//start_color();
	for(k=1;k<BLOCK_NUM;k++)
	{
		for( i = 0; i < 4; i++ ){
			move(4+i+(k-1)*7,WIDTH+13);
			for( j = 0; j < 4; j++ ){
				if( block[nextBlock[k]][0][i][j] == 1 ){
					attron(A_REVERSE);
					//attron(COLOR_PAIR(nextBlock[k]));
					printw(" ");
					//attroff(COLOR_PAIR(nextBlock[k]));
					attroff(A_REVERSE);
				}
				else printw(" ");
			}
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
				//move(HEIGHT+2,WIDTH);
				//printw("Block: %d %d %d %d\n",y,x,blockID,blockRotate);
			}
		}
	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch('o');
	//addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
	//	addch(ACS_HLINE);
	//addch(ACS_URCORNER);
	addch('-');
	addch('o');
	for(j=0;j<height;j++){
		move(y+j+1,x);
		//addch(ACS_VLINE);
		addch('|');
		move(y+j+1,x+width+1);
		//addch(ACS_VLINE);
		addch('|');
	}
	move(y+j+1,x);
	//addch(ACS_LLCORNER);
	addch('o');
	for(i=0;i<width;i++)
		addch('-');
	//	addch(ACS_HLINE);
	//addch(ACS_LRCORNER);
	addch('o');
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);
	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i,j;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			if(block[currentBlock][blockRotate][i][j])
			{
				if( (blockY+i>=HEIGHT) || (blockX+j<0) || (blockX+j>=WIDTH) ) return 0;
				else if(f[blockY+i][blockX+j]==1) return 0;
			}	
		}	
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i,j;
	int rot=blockRotate, y=blockY, x=blockX;
	int SY;
	if(command==KEY_UP) blockRotate=(blockRotate+3)%4;
	else if(command==KEY_LEFT) blockX=blockX+1;
	else if(command==KEY_RIGHT) blockX=blockX-1;
	else if(command==KEY_DOWN) blockY=blockY-1;
	SY=blockY;
	while(CheckToMove(f,currentBlock,blockRotate,++SY,blockX));
	SY--;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			if(block[currentBlock][blockRotate][i][j])
			{
				if(i+blockY>=0)
				{
					move(i+blockY+1,j+blockX+1);
					printw(".");
				}
				if(i+SY>=0)
				{
					move(i+SY+1,j+blockX+1);
					printw(".");
				}
			}
		}
	}
				
//	DrawBlock(y,x,currentBlock,rot,' ');
	DrawBlockWithFeatures(y,x,currentBlock,rot);
	move(HEIGHT,WIDTH+10);
}

void BlockDown(int sig){
	// user code
	int i,j;
	int chk=0;
	/*	
	if(command==' ')
	{
		while(1)
		{
			if(CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)==0) break;
			blockY+=limit;
			DrawChange(field,KEY_DOWN,nextBlock[0],blockRotate,blockY,blockX);	
		}
		
	}
	*/
	if(CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX))
	{
		blockY++;
		DrawChange(field,KEY_DOWN,nextBlock[0],blockRotate,blockY,blockX);		
	}
	else
	{
		if(blockY==-1) gameOver=1;
		score+=AddBlockToField(field,nextBlock[0],blockRotate,blockY,blockX);
		score+=DeleteLine(field);
		for(i=0;i<VISIBLE_BLOCKS-1;i++)
			nextBlock[i]=nextBlock[i+1];
		nextBlock[VISIBLE_BLOCKS-1]=rand()%7;
		blockRotate=0;
		blockY=-1;
		blockX=WIDTH/2-2;
		

		recommend(recRoot);
		PrintScore(score);
		DrawNextBlock(nextBlock);
		DrawField();
		DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	}
	timed_out=0;
}
int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i,j;
	int touched=0;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			if(block[currentBlock][blockRotate][i][j])
			{
				if(blockY+i==HEIGHT-1 || f[blockY+i+1][blockX+j]==1) touched++;
				f[blockY+i][blockX+j]=1;
				
			}
		}
	}
	//score+=touched*10;
	return touched*10;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code
	int Del_lines_num=0;
	int i,j,p,q;
	int chk=0;
	for(i=1;i<HEIGHT;i++)
	{
		chk=0;
		for(j=0;j<WIDTH;j++)
		{
			if(f[i][j]==0) 
			{
				chk=1; 
				break;
			}
		}
		if(chk==0)
		{
			Del_lines_num++; 
			for(p=i;p>0;p--)
				for(q=0;q<WIDTH;q++)
					f[p][q]=f[p-1][q];
			//DrawField();
		}
	}
	return Del_lines_num*Del_lines_num*100;
}

void DrawShadow(int y, int x, int blockID,int blockRotate){
	// user code
	/*
	int chk=0,max_i=0;
	int k,i,j;
	for(k=1;k<HEIGHT-y;k++)
	{
		for(i=0;i<4;i++)
		{
			for(j=0;j<4;j++)
			{
				if(block[blockID][blockRotate][i][j])
				{
					if(y+i+k<HEIGHT)
					{
						if(field[y+i+k][x+j])
						{
							limit=k-1;
							chk++;
						}
					}
					if(k==1)
						if(max_i<i) max_i=i;
				}
			}
		}
		if(chk) break;
				
	}
	if(chk==0)
		limit=HEIGHT-y-max_i-1;
	if(y+limit+max_i>HEIGHT-1)
		limit--;
	DrawField();
	*/
	while(CheckToMove(field,blockID,blockRotate,++y,x));
	DrawBlock(--y,x,blockID,blockRotate,'/');
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate)
{
	DrawRecommend();
	DrawShadow(y,x,blockID,blockRotate);
	DrawBlock(y,x,blockID,blockRotate,' ');
	//DrawRecommend();
}

void CreateRankNode(char str[],int scr)
{
	int i;
	nptr new_node;
	new_node=(nptr)malloc(sizeof(NODE));
	strcpy(new_node->name,str);
	new_node->score=scr;
	new_node->link=NULL;
	if(ranking==NULL)
	{
		ranking=new_node;
		lastnode=new_node;
	}
	else
	{
		lastnode->link=new_node;
		lastnode=new_node;
	}
}

void InsertRankNode(char str[],int scr)
{
	int i;
	nptr tmp=ranking;
	nptr tmp2=ranking;
	nptr new_node=(nptr)malloc(sizeof(NODE));
	strcpy(new_node->name,str);
	new_node->score=scr;
	new_node->link=NULL;
	if(ranking==NULL) ranking=new_node;
	else
	{
		while(1)
		{
			if(new_node->score > tmp->score)
			{
				if(tmp==ranking)
				{
					new_node->link=tmp;
					ranking=new_node;
					break;
				}
				else
				{
					new_node->link=tmp;
					tmp2->link=new_node;
					break;
				}
			}
			if(tmp->link==NULL)
			{
				tmp->link=new_node;
				break;
			}
			tmp2=tmp;
			tmp=tmp->link;
		}
	}
}

void DeleteRankNode(nptr *a)
{
	nptr del=NULL;
	if((*a)!=NULL)
	{
		del=(*a);
		(*a)=(*a)->link;
		free(del);
	}
}

void createRankList(){
	// user code
	int tmp_score=0, i=0;
	char tmp_name[NAMELEN];
	FILE *fp=fopen("rank.txt","r");
	if(ranking!=NULL)
	{
		for(i=0;i<rank_num;i++) DeleteRankNode(&ranking);
	}
	fscanf(fp,"%d",&rank_num);
	for(i=0;i<rank_num;i++)
	{
		//printw("i: %d\n",i);
		fscanf(fp,"%s %d\n",tmp_name,&tmp_score);
		CreateRankNode(tmp_name,tmp_score);
	}
	fclose(fp);
}

void rank(){
	// user code
	int x,y,i,chk=0,del_rank;
	int count;
	char select;
	nptr tmp=ranking;
	nptr tmp2=NULL;
	nptr pretmp=NULL;
	char del_name[NAMELEN]={0,};
	x=1;
	y=rank_num;
	clear();
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");
	select=wgetch(stdscr);
	if(select=='1')
	{
		tmp=ranking;
		echo();
		printw("X: \n");
		printw("Y: \n");
		move(3,3);
		scanw("%d",&x);
		move(4,3);
		scanw("%d",&y);
		noecho();
		printw("\tname\t|\tscore\n");
		printw("---------------------------------------\n");
		if(x>y) printw("search failure: no rank in the list\n");
		else
		{
			if(y>rank_num) y=rank_num;
			for(i=1;i<=rank_num;i++)
			{
				if(i>=x && i<=y)
					printw(" %s\t\t| %d\n",tmp->name,tmp->score);
				else if(i>y) break;
				tmp=tmp->link;
			}		
		}
		getch();
	}
	else if(select=='2')
	{
		tmp=ranking;
		chk=0;
		echo();
		printw("Input the name: ");
		scanw("%s",del_name);
		noecho();
		printw("\tname\t|\tscore\n");
		printw("---------------------------------------\n");
		for(;tmp!=NULL;tmp=tmp->link)
		{
			if(strcmp(tmp->name,del_name)==0)
			{
				chk++;
				printw("%s\t\t| %d\n",tmp->name,tmp->score);
			}
		}
		if(!chk) printw("search failure: no information in the list\n");
		getch();
	}
	else if(select=='3')
	{
		echo();
		printw("Input the rank: ");
		scanw("%d",&del_rank);
		noecho();
		tmp=ranking;
		if(del_rank>rank_num)
		{
			printw("search failure: no rank in the list\n");
			getch();
		}
		else
		{
			count=0;
			for(;tmp!=NULL;tmp=tmp->link)
			{
				count++;
				if(count==del_rank)
				{
					if(count==1)
					{
						tmp2=ranking;
						ranking=ranking->link;
						free(tmp2);
					}
					else
					{
						tmp2=tmp;
						pretmp->link=tmp->link;
						free(tmp2);
					}
					rank_num--;
					writeRankFile();
					printw("result: the rank deleted\n");
					getch();
					break;
				}
				pretmp=tmp;
			}
		}
	}
}

void writeRankFile(){
	// user code
	FILE *ofp=fopen("rank.txt","w");
	int i;
	nptr tmp=ranking;
	fprintf(ofp,"%d\n",rank_num);
	for(i=0;i<rank_num;i++)
	{
		fprintf(ofp,"%s %d\n",tmp->name,tmp->score);
		if(tmp->link==NULL) break;
		tmp=tmp->link;
	}
	fclose(ofp);
}

void newRank(int score){
	// user code
	char user_name[NAMELEN];
	rank_num+=1;
	clear();
	echo();
	printw("your name: ");
	scanw("%s",user_name);
	noecho();
	InsertRankNode(user_name,score);
	writeRankFile();
}

void DrawRecommend(){
	// user code
	//if(CheckToMove(field,nextBlock[0],recommendR,recommendY,recommendX)==1)
	DrawBlock(recommendY,recommendX,nextBlock[0],recommendR,'R');
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
	int r,x,y;
	int level=root->lv+1;
	int Rbound,Lbound;
	int h,w,calc;
	int i=0,min_count=50,temp_count=50,max_y=0;
	int recR,recY,recX;
	int recommended=0;
	RecNode **c=root->child;
	for(r=0;r<4;r++)
	{
		//if(nextBlock[level]==0) r=3;
		Rbound=0;
		for(h=0;h<4;h++)
		{
			for(w=3;w>=0;w--)
			{
				if(block[nextBlock[level]][r][h][w]) break;
			}
			if(w>Rbound) Rbound=w;
		}
		Rbound=WIDTH-1-Rbound;
		Lbound=3;
		for(h=0;h<4;h++)
		{
			for(w=0;w<4;w++)
			{
				if(block[nextBlock[level]][r][h][w]) break;
			}
			if(w<Lbound) Lbound=w;
		}	
		Lbound=0-Lbound;		
		for(x=Lbound;x<=Rbound;++x,++i)
		{
			for(h=0;h<HEIGHT;h++)
				for(w=0;w<WIDTH;w++)
					c[i]->recField[h][w]=root->recField[h][w];


			y=0;
			if(CheckToMove(c[i]->recField,nextBlock[level],r,y,x)){
				while(CheckToMove(c[i]->recField,nextBlock[level],r,++y,x));
				y--;
			}
			else{ 
				continue;
			}

			c[i]->score=root->score + AddBlockToField(c[i]->recField,nextBlock[level],r,y,x) + DeleteLine(c[i]->recField);

			if(level < VISIBLE_BLOCKS-1) calc=recommend(c[i]);
			else calc=c[i]->score;

			temp_count=CountBlank(c[i]->recField);
			if(min_count>=temp_count){
				min_count=temp_count;
				if(max_y<y){
					max_y=y;
					recommended=1;
					recR=r;
					recY=y;
					recX=x;
				}
				if(max<calc){
					recommended=1;
					max=calc;
					recR=r;
					recY=y;
					recX=x;
					//move(HEIGHT+10,0);
					//printw("rec: %d %d %d\n",r,y,x);
				}

			}
		}
	}
	if(level==0 && recommended==1){
		recommendR=recR;
		recommendY=recY;
		recommendX=recX;
		//move(HEIGHT+10,0);
		//printw("recY: %d, recX: %d, recR %d, score: %d\n",recY,recX,recR,max);
	}
	// user code

	return max;
}

void makeTree(RecNode *root)
{
	int i,p;

	for(i=0;i<CHILDREN_MAX;i++){
		root->child[i]=(RecNode *)malloc(sizeof(RecNode));
		root->child[i]->lv=root->lv + 1;
		root->child[i]->recField=(char(*)[WIDTH])malloc(sizeof(char)*HEIGHT*WIDTH);
		if(root->child[i]->lv < VISIBLE_BLOCKS)
			makeTree(root->child[i]);
	}

}

void BlockDown_REC(int sig){
	int i;
	if(CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX)==0) gameOver=1;
	else{
		score+=AddBlockToField(field,nextBlock[0],recommendR,recommendY,recommendX);
		score+=DeleteLine(field);
		blockY=-1;
		blockX=(WIDTH/2)-2;
		blockRotate=0;
		for(i=0;i<VISIBLE_BLOCKS-1;i++)
			nextBlock[i]=nextBlock[i+1];
		nextBlock[VISIBLE_BLOCKS-1]=rand()%7;
		//if(nextBlock[VISIBLE_BLOCKS-1]==7) nextBlock[VISIBLE_BLOCKS-1]=0;
		recommend(recRoot);
		DrawNextBlock(nextBlock);
		PrintScore(score);
		DrawField();
		DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
		timed_out=0;
	}
}

void recommendedPlay(){
	// user code
	int command;
	clear();
	act.sa_handler = BlockDown_REC;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}
		command=GetCommand();
		if(command==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();
			return;
		}
	}while(!gameOver);
	
	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOVer!!");
	refresh();
	getch();
}

int CountBlank(char f[HEIGHT][WIDTH])
{
	int chk=0,count=0;
	int i,j;
	for(j=0;j<WIDTH;j++)
	{
		chk=0;
		for(i=0;i<HEIGHT;i++)
		{
			if(f[i][j]==1) chk=1;
			if(chk==1 && f[i][j]==0) count++;
		}
	}
	return count;
}



