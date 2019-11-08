#include "Factory/RModelBuilder.h"


// Sets default values
URModelBuilder::URModelBuilder()
{
  LinkFactory = CreateDefaultSubobject<URLinkFactory>(TEXT("LinkFactory"));
  JointFactory = CreateDefaultSubobject<URJointFactory>(TEXT("JointFactory"));
}

// Dtor
URModelBuilder::~URModelBuilder()
{
}

// Load model
void URModelBuilder::Load(USDFModel* InModelDescription, ARModel* OutModel)
{
  ModelDescription = InModelDescription;
  if(OutModel)
    {
      Model = OutModel;
      LoadLinks();
      LoadJoints();
      BuildKinematicTree();
    }
}

// Load links
void URModelBuilder::LoadLinks()
{
  for(USDFLink* Link : ModelDescription->Links)
    {
      URLink* TempLink = LinkFactory->Load(Model, Link);
      if(TempLink)
        {
          if(!Model->BaseLink)
            {
              Model->BaseLink = TempLink;
            }
          Model->AddLink(TempLink);
        }
      else
        {
          UE_LOG(LogTemp, Error, TEXT("Creation of Link %s failed"), *Link->GetName());
        }
    }
}

// Load joints
void URModelBuilder::LoadJoints()
{
  for(USDFJoint* Joint : ModelDescription->Joints)
    {
      URJoint* TempJoint = JointFactory->Load(Model, Joint);
      if(TempJoint)
        {
          Model->AddJoint(TempJoint);
        }
      else
        {
          UE_LOG(LogTemp, Error, TEXT("Creation of Joint %s failed"), *Joint->GetName());
        }
    }
}

void URModelBuilder::BuildKinematicTree()
{
  for(auto& Joint : Model->Joints)
    {
      URLink* Parent = *Model->Links.Find(Joint.Value->ParentName);
      URLink* Child = *Model->Links.Find(Joint.Value->ChildName);
      if(!Parent)
        {
          UE_LOG(LogTemp, Error, TEXT("Parent %s not found"), *Joint.Value->ParentName);
          break;
        }
      if(!Child)
        {
          UE_LOG(LogTemp, Error, TEXT("Child %s not found"), *Joint.Value->ChildName);
          break;
        }

      Joint.Value->SetParentChild(Parent, Child);
      SetConstraintPosition(Joint.Value);
      Joint.Value->Constraint->ConnectToComponents();

      // if(!Joint.Value->Child->Parent->FindComponentsByClass<URStaticMeshComponent>().Contains(Joint.Value->Child->GetCollision()))

      if(!Joint.Value->Child->bAttachedToParent)
        {
          Joint.Value->Child->GetCollision()->AttachToComponent(Joint.Value->Parent->GetCollision(), FAttachmentTransformRules::KeepWorldTransform);
          Joint.Value->Child->bAttachedToParent = true;
          // if(Joint.Value->Child->GetName().Equals(TEXT("fr_caster_l_wheel_link")))
          //   {
          //   }
          // UE_LOG(LogTemp, Error, TEXT("Joint: %s Parent: %s Child: %s"), *Joint.Value->GetName(), *Joint.Value->Parent->GetCollision()->GetName(), *Joint.Value->Child->GetCollision()->GetName());
        }
      else
        {
          Joint.Value->Parent->GetCollision()->AttachToComponent(Joint.Value->Child->GetCollision(), FAttachmentTransformRules::KeepWorldTransform);
          Joint.Value->Parent->bAttachedToParent = true;
        }
      Parent->AddJoint(Joint.Value);
    }
}

void URModelBuilder::SetConstraintPosition(URJoint* InJoint)
{
  if(InJoint->bUseParentModelFrame)
    {
      // InJoint->Constraint->AttachToComponent(InJoint->Parent->GetCollision(), FAttachmentTransformRules::KeepWorldTransform);
      InJoint->Constraint->AttachToComponent(InJoint->Child->GetCollision(), FAttachmentTransformRules::KeepWorldTransform);
      InJoint->Constraint->SetWorldLocation(InJoint->Child->GetCollision()->GetComponentLocation());
      InJoint->Constraint->AddRelativeLocation(InJoint->Pose.GetLocation());
      InJoint->Constraint->AddRelativeRotation(InJoint->Pose.GetRotation());
    }
  else
    {
      //TODO Implement and check this case
      UE_LOG(LogTemp, Warning, TEXT("Does't use parent frame"));

      // InJoint->Constraint->SetPosition(InJoint);
      InJoint->Constraint->AttachToComponent(InJoint->Child->GetCollision(), FAttachmentTransformRules::KeepRelativeTransform);
    }
}
