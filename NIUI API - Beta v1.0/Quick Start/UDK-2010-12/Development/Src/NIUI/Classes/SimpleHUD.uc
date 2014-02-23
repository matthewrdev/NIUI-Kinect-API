class SimpleHUD extends Object;

var string UserState;
var HUD TargetHUD;

function DrawMyHud(HUD theHUD)
{
	targetHud = theHUD;
	DrawUserState();
}

function DrawUserState()
{
	local Vector Pos;

	pos.X = 0.0f;
	pos.Y = TargetHUD.SizeY - 100.0f;
	DrawStringToHUD(UserState, Pos, targetHUD.Canvas, AsColor(255, 255,255,255));
}

function Color AsColor(BYTE A, BYTE R, BYTE G, BYTE B)
{
	local Color col;

	col.A = A;
	col.R = R;
	col.G = G;
	col.B = B;

	return col;
}


function DrawStringToHUD(string text, Vector pos, Canvas theCanvas, Color textColor)
{
	theCanvas.SetDrawColor(textColor.R, textColor.G, textColor.B, textColor.A);
	theCanvas.SetPos(pos.X, pos.Y, 1.0f);
	theCanvas.DrawText(text,,5.0f, 5.0f);
}

DefaultProperties
{
}
