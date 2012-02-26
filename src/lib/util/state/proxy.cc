
#ifdef __GNUG__
#pragma implementation
#endif

#include <util/state/state_bin.h>
#include <util/state/proxy.h>
#include <util/keyval/keyval.h>

using namespace sc;

static ClassDesc SavableStateProxy_cd(
    typeid(SavableStateProxy),
    "SavableStateProxy",1,"public DescribedClassProxy",
    0,create<SavableStateProxy>);

SavableStateProxy::SavableStateProxy(const Ref<KeyVal> &keyval)
{
  Ref<StateIn> statein; statein << keyval->describedclassvalue("statein");
  if (statein.nonnull()) {
      char *objectname = keyval->pcharvalue("object");
      StateIn &si = *(statein.pointer());
      if (keyval->exists("override")) {
          si.set_override(new PrefixKeyVal(keyval,"override"));
        }
      if (objectname) {
          object_ = SavableState::dir_restore_state(si, objectname);
          delete[] objectname;
        }
      else {
          object_= SavableState::restore_state(si);
        }
    }
}

Ref<DescribedClass>
SavableStateProxy::object()
{
  return object_.pointer();
}

