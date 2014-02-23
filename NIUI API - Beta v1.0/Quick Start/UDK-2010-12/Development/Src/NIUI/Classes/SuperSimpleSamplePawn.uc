class SuperSimpleSamplePawn extends UDKPawn;

//var NIUI_SkeletalController niuiSkelControl;
//var array<name> TargetBoneNames;

//var AnimNodePlayCustomAnim customAnim;

//simulated function PostInitAnimTree(SkeletalMeshComponent SkelComp)
//{
//	super.PostInitAnimTree(SkelComp);

//	// Add the spine control name as a node to be cached by the SkeletalController
//	TargetBoneNames.AddItem('Spine');

//	niuiSkelControl = Spawn(class'NIUI_SkeletalController',,, Location);
//	niuiSkelControl.CacheSkeletonControls(SkelComp, TargetBoneNames);

//	// FInd the base animation blender and the crossfade blender.
//	customAnim = AnimNodePlayCustomAnim(AnimBlendList.FindAnimNode('CustomAnim'));
//}

//function InitiliseSkeletalControls(int NIUI_UserID, NIUI_Core core)
//{
//	niuiSkelControl.CacheRotations(Mesh, self.Rotation);
//	niuiSkelControl.Bind(core, NIUI_UserID);
//}

//function UpdateSkeletalControls(float DeltaTime)
//{
//	niuiSkelCOntrol.UpdateBoneOverides(DeltaTIme, self.Location, Mesh, self.Rotation);
//}

function PlayAnim(name theName)
{

}


DefaultProperties
{
}
