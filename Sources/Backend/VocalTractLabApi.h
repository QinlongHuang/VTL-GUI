#include <vector>
#include <string>
#include <new>  // For std::nothrow

#include "AnatomyParams.h"
#include "VocalTract.h"
#include "TdsModel.h"
#include "TlModel.h"
#include "Synthesizer.h"
#include "VocalTractPicture.h"

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
    VocalTractPicture *vtPicture;

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
    int vtlSetAnatomyParams2(double* params);
    vector<double> vtlGetAnatomyParams();
    vector<double> vtlSynthAudio(vector<double> tractParams, vector<double> glottisParams, int numFrames,
        int frameStep_samples);
    vector<double> vtlTract2EMA(vector<double> tractParams, int numFrames);
    vector<string> vtlGetEMANames();
    int vtlExportTractSvg(vector<double> tractParams, const char *fileName, bool addCenterLine = false, bool addCutVectors = false);

    int vtlSaveTractFrame(double* tractParams, const char *fileName);
    int vtlSaveTractVideo(int numFrames, double* tractParams, const char *folderName);
};

#ifdef __cplusplus
extern "C"{
#endif

#ifdef WIN32
  #ifdef _USRDLL
    #define C_EXPORT __declspec(dllexport)
  #else
    #define C_EXPORT
  #endif        // DLL
#else
  #define C_EXPORT
#endif  // WIN32

C_EXPORT VocalTractLab* VTL_new(const char *speakerFileName);
C_EXPORT int setAnatomy(VocalTractLab* ptr, double *params);
C_EXPORT int exportVTFrame(VocalTractLab* ptr, double* tractParams, const char *fileName);
C_EXPORT int exportVTVideo(VocalTractLab* ptr, int numFrames, double* tractParams, const char *folderName);
C_EXPORT void VTL_delete(VocalTractLab* ptr);

#ifdef __cplusplus
}
#endif
