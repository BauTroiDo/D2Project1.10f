#ifndef __EXSCROLLBAR_H__
#define __EXSCROLLBAR_H__

class ExScrollBar : public ExControl
{
public:
	ExScrollBar(int X, int Y, int Min, int Max, int Height,int *Var, void (*efnOnChange)(ExControl*));
	~ExScrollBar();
	void Draw();
	void Update();
	bool isPressed(unsigned int Sender,WPARAM wParam);
	void SetHeight(int cH);
	void SetState(States aState);
	int sMin;
	int sMax;
	int* sValue;
private:
	void (*fnOnChange)(ExControl*);
	ExButton *Up;
	ExButton *Down;
	ExRectangle *Slider;
	ExCellFile *aCellFile;
};



#endif
