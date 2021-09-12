#include <vector>
#include <string>

#include "AnatomyParams.h"
#include "VocalTract.h"
#include "TdsModel.h"
#include "TlModel.h"
#include "Synthesizer.h"

#include "GeometricGlottis.h"
#include "TwoMassModel.h"
#include "TriangularGlottis.h"


using namespace std;

enum GlottisModel
{
  GEOMETRIC_GLOTTIS,
  TWO_MASS_MODEL,
  TRIANGULAR_GLOTTIS,
  NUM_GLOTTIS_MODELS
};

class VocalTractLab
{
  private:
    Glottis *glottis[NUM_GLOTTIS_MODELS];
    int selectedGlottis;

    VocalTract *vocalTract;
    TdsModel *tdsModel;
    Synthesizer *synthesizer;
    Tube *tube;
    AnatomyParams *anatomyParams;

    bool vtlLoadSpeaker(string speakerFileName, VocalTract *vocalTract,
      Glottis *glottis[], int &selectedGlottis);
    int vtlSynthesisReset();

  public:
    VocalTractLab(const string speakerFileName);
    ~VocalTractLab();

    int vtlInitialize(const char *speakerFileName);
    int vtlClose();
    vector<double> vtlGetTractParamInfo();
    vector<double> vtlGetGlottisParamInfo();
    int vtlSetAnatomyParams(vector<double> params);
    vector<double> vtlGetAnatomyParams();
    vector<double> vtlSynthAudio(vector<double> tractParams, vector<double> glottisParams, int numFrames,
        int frameStep_samples);

};

