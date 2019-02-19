#include "DLLmain.h"

ExWindow::ExWindow(int wX, int wY, int wWid, int wHei, bool isTopBar, wstring szLabel) : ExControl(wX, wY, wWid, wHei, 0)
{
	bisTopBar = isTopBar;
	Label = szLabel;
	bDrawText = true;
	wTextColor = 8;
	wBarColor = 0xE9;
	Child.clear();
	TextWidth = TextHeight = DragX = DragY = 0;
	if (bisTopBar) bMoveable = true;

	if (!Label.empty()) {
		for (int i = 0; i < 10; i++)
		{
			D2WIN_SetTextSize(FontTable[i]);
			TextWidth = D2WIN_GetTextWidth(Label.c_str());
			aFont = FontTable[i];
			if (aFont == 3) TextHeight = 35; else if (aFont == 6) TextHeight = 12; else TextHeight = D2WIN_GetFontHeight();
			if (cX + TextWidth < (cX + cWidth) && (cY + TextHeight) < (isTopBar ? cY + 15 : (cY + cHeight)))  break;
			if (i == 9){ bDrawText = false; break; }
		}
	}
	else bDrawText = false;

	D2CLIENT_BlockedRect->left = cX;
	D2CLIENT_BlockedRect->top = cY;
	D2CLIENT_BlockedRect->right = cX + cWidth;
	D2CLIENT_BlockedRect->bottom = cY + cHeight;
	*D2CLIENT_BlockScreen = true;
}

void ExWindow::Draw()
{
	if (cState == VISIBLE || cState == DISABLED)
	{
		int aColor = cState == 1 ? bBeingPressed ? 0x11 : wBarColor : 4;
		int aColorText = cState == 1 ? bBeingPressed ? bisTopBar ? 9 : wTextColor : wTextColor : 15;
		D2GFX_DrawRectangle(cX, cY, cX + cWidth, cY + cHeight, 0xF, 1);
		D2CLIENT_DrawGZBox(cX, cY, cX + cWidth, cY + cHeight);
		if (bisTopBar)
		{
			D2GFX_DrawRectangle(cX, cY, cX + cWidth, cY + 15, aColor, 2);
			D2CLIENT_DrawGZBox(cX, cY, cX + cWidth, cY + 15);
		}
		if (bDrawText)
		{
			int TextX = ((cX + cX + cWidth) - TextWidth) / 2;
			int TextY = bisTopBar ? ((cY + cY + 15) + TextHeight) / 2 : cY + TextHeight;
			D2WIN_SetTextSize(aFont);
			if (!Label.empty())	D2WIN_D2DrawText(Label.c_str(), TextX, TextY, aColorText, 0);
		}
	}
}

void ExWindow::SetAlign(Align xAlign, Align yAlign)
{
	wAlign = xAlign;
	hAlign = yAlign;

	if(xAlign != NONE || yAlign != NONE) Relocate();
}

void ExWindow::Relocate() //- Set control align
{
	if (wAlign == CENTER) cX = (*D2CLIENT_ScreenWidth - cWidth) / 2;
	else if (wAlign == RIGHT) cX = (*D2CLIENT_ScreenWidth - cWidth);
	if (hAlign == CENTER) cY = (*D2CLIENT_ScreenHeight - cHeight) / 2;
	else if (hAlign == RIGHT) cY = *D2CLIENT_ScreenHeight - cHeight;

	D2CLIENT_BlockedRect->left = cX;
	D2CLIENT_BlockedRect->top = cY;
	D2CLIENT_BlockedRect->right = cX + cWidth;
	D2CLIENT_BlockedRect->bottom = cY + cHeight;
}

void ExWindow::Resize(int NewWidth, int NewHeight)
{
	D2CLIENT_BlockedRect->right = cX + NewWidth;
	D2CLIENT_BlockedRect->bottom = cY + NewHeight;
	cHeight = NewHeight;
	cWidth = NewWidth;
}

void ExWindow::AddChild(ExControl* ptChild)
{
	Child.push_back(ptChild);
	ptChild->ptParent = this;
}

void ExWindow::DeleteChild(ExControl* ptChild)
{
	for (deque<ExControl*>::size_type i = 0; i < Child.size(); ++i)
	{
		if (Child.at(i) == ptChild) { Child.at(i) = 0; ptChild->ptParent = 0; break; }
	}
}

bool ExWindow::isPressed(unsigned int Sender, WPARAM wParam)
{
	if (cState == INVISIBLE) return false;
	switch (Sender)
	{
	case WM_LBUTTONDOWN:
		if (*D2CLIENT_MouseX >= cX && *D2CLIENT_MouseX <= cX + cWidth && *D2CLIENT_MouseY >= cY && *D2CLIENT_MouseY <= cY + cHeight)
		{
			bBeingPressed = true;
			if (*D2CLIENT_MouseY <= cY + 15)
			{
				if (!bMoveable) return true;
				DragX = *D2CLIENT_MouseX;
				DragY = *D2CLIENT_MouseY;
				OldX = cX;
				OldY = cY;
				for (vector<ExControl*>::size_type i = 0; i < Child.size(); ++i)
				{
					if (Child.at(i) == 0) continue;
					Child.at(i)->OldX = Child.at(i)->GetX();
					Child.at(i)->OldY = Child.at(i)->GetY();
				}
				return true;
			}
		}
		break;
	case WM_LBUTTONUP:
		if (*D2CLIENT_MouseX >= cX && *D2CLIENT_MouseX <= cX + cWidth && *D2CLIENT_MouseY >= cY && *D2CLIENT_MouseY <= cY + cHeight)
		{
			D2CLIENT_BlockedRect->left = cX;
			D2CLIENT_BlockedRect->top = cY;
			D2CLIENT_BlockedRect->right = cX + cWidth;
			D2CLIENT_BlockedRect->bottom = cY + cHeight;
			bBeingPressed = false;
			return true;
		}
		break;
	case WM_MOUSEMOVE:
		if (!(wParam & (MK_LBUTTON | MK_CONTROL))) break;
		if (*D2CLIENT_MouseX >= cX && *D2CLIENT_MouseX <= cX + cWidth && *D2CLIENT_MouseY >= cY && *D2CLIENT_MouseY <= cY + cHeight)
		{
			if (wParam & MK_LBUTTON) bBeingPressed = true;
			if (bBeingPressed && *D2CLIENT_MouseY <= cY + 15 && (wParam & MK_CONTROL) && bMoveable && DragX)
			{
				signed int xOffset = (*D2CLIENT_MouseX - DragX);
				signed int yOffset = (*D2CLIENT_MouseY - DragY);
				cX = OldX + xOffset;
				cY = OldY + yOffset;
				for (vector<ExControl*>::size_type i = 0; i < Child.size(); ++i)
				{
					if (Child.at(i) == 0) continue;
					Child.at(i)->SetX(Child.at(i)->OldX + xOffset);
					Child.at(i)->SetY(Child.at(i)->OldY + yOffset);
				}
			}
		}
		else
			bBeingPressed = false;
		break;
	}
	return false;
}

ExWindow::~ExWindow(void)
{
	*D2CLIENT_BlockScreen = false;
}