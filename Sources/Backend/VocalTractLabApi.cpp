#include "VocalTractLabApi.h"

#include "GesturalScore.h"
#include "XmlHelper.h"
#include "XmlNode.h"
#include "TlModel.h"
#include "Geometry.h"


#include <iostream>
#include <fstream>
#include <string>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include "bitmap_image.hpp"

using namespace std;

// // For OpenGL rendering
// static VocalTract *vTract;
// static VocalTractPicture *vtPicture;

VocalTractLab::VocalTractLab(const string speakerFileName)
{
  // ****************************************************************
  // Init the vocal tract.
  // ****************************************************************
  vocalTract = new VocalTract();
  vocalTract->calculateAll();
  // vTract = vocalTract;

  // ****************************************************************
  // Init the list with glottis models
  // ****************************************************************

  glottis[GEOMETRIC_GLOTTIS] = new GeometricGlottis();
  glottis[TWO_MASS_MODEL] = new TwoMassModel();
  glottis[TRIANGULAR_GLOTTIS] = new TriangularGlottis();

  selectedGlottis = GEOMETRIC_GLOTTIS;

  bool ok = vtlLoadSpeaker(speakerFileName, vocalTract, glottis, selectedGlottis);

  if (ok == false)
  {
    int i;
    for (i = 0; i < NUM_GLOTTIS_MODELS; i++)
    {
      delete glottis[i];
    }
    delete vocalTract;

    throw runtime_error("Error in vtlInitialize(): vtlLoadSpeaker() failed.\n");
  }

  // ****************************************************************
  // Init the object for the time domain simulation.
  // ****************************************************************
  tdsModel = new TdsModel();

  // ****************************************************************
  // Init the Synthesizer object.
  // ****************************************************************

  synthesizer = new Synthesizer();
  synthesizer->init(glottis[selectedGlottis], vocalTract, tdsModel);

  tube = new Tube();

  anatomyParams = new AnatomyParams();

  // ****************************************************************
  // Init the Vocal Tract Picture.
  // ****************************************************************
  vtPicture  = new VocalTractPicture(vocalTract);
}

bool VocalTractLab::vtlLoadSpeaker(string speakerFileName, VocalTract *vocalTract, 
  Glottis *glottis[], int &selectedGlottis)
{
  // ****************************************************************
  // Load the XML data from the speaker file.
  // ****************************************************************

  vector<XmlError> xmlErrors;
  XmlNode *rootNode = xmlParseFile(speakerFileName, "speaker", &xmlErrors);
  if (rootNode == NULL)
  {
    xmlPrintErrors(xmlErrors);
    return false;
  }

  // ****************************************************************
  // Load the data for the glottis models.
  // ****************************************************************

  // This may be overwritten later.
  selectedGlottis = GEOMETRIC_GLOTTIS;

  XmlNode *glottisModelsNode = rootNode->getChildElement("glottis_models");
  if (glottisModelsNode != NULL)
  {
    int i;
    XmlNode *glottisNode;

    for (i=0; (i < (int)glottisModelsNode->childElement.size()) && (i < NUM_GLOTTIS_MODELS); i++)
    {
      glottisNode = glottisModelsNode->childElement[i];
      if (glottisNode->getAttributeString("type") == glottis[i]->getName())
      {
        if (glottisNode->getAttributeInt("selected") == 1)
        {
          selectedGlottis = i;
        }
        if (glottis[i]->readFromXml(*glottisNode) == false)
        {
          printf("Error: Failed to read glottis data for glottis model %d!\n", i);
          delete rootNode;
          return false;
        }
      }
      else
      {
        printf("Error: The type of the glottis model %d in the speaker file is '%s' "
          "but should be '%s'!\n", i, 
          glottisNode->getAttributeString("type").c_str(), 
          glottis[i]->getName().c_str());

        delete rootNode;
        return false;
      }
    }
  }
  else
  {
    printf("Warning: No glottis model data found in the speaker file %s!\n", speakerFileName.c_str());
  }

  // Free the memory of the XML tree !
  delete rootNode;

  // ****************************************************************
  // Load the vocal tract anatomy and vocal tract shapes.
  // ****************************************************************

  try
  {
    vocalTract->readFromXml(speakerFileName);
    vocalTract->calculateAll();
  }
  catch (std::string st)
  {
    printf("%s\n", st.c_str());
    printf("Error reading the anatomy data from %s.\n", speakerFileName.c_str());
    return false;
  }

  return true;
}

int VocalTractLab::vtlClose()
{
  delete synthesizer;
  delete tdsModel;

  int i;
  for (i = 0; i < NUM_GLOTTIS_MODELS; i++)
  {
    delete glottis[i];
  }

  delete vocalTract;

  delete anatomyParams;

  return 0;
}

VocalTractLab::~VocalTractLab()
{
  vtlClose();
}

vector<double> VocalTractLab::vtlGetGlottisParamInfo()
{
  int i;
  int numGlottisParams = (int)glottis[selectedGlottis]->controlParam.size();

  string name(32*11, ' ');
  char *names = const_cast<char*>(name.c_str());

  strcpy(names, "");

  vector<double> glottis_param;
  glottis_param.resize(3 * numGlottisParams);

  for (i=0; i < numGlottisParams; i++)
  {
    strcat(names, glottis[selectedGlottis]->controlParam[i].abbr.c_str());
    if (i != VocalTract::NUM_PARAMS - 1)
    {
      strcat(names, " ");
    }
    //[min...max...neutral]
    glottis_param[i] = glottis[selectedGlottis]->controlParam[i].min;
    glottis_param[i+numGlottisParams] = glottis[selectedGlottis]->controlParam[i].max;
    glottis_param[i+(2*numGlottisParams)] = glottis[selectedGlottis]->controlParam[i].neutral;
  }

  return glottis_param;
}

vector<double> VocalTractLab::vtlGetTractParamInfo()
{
  int i;

  string name(32*19, ' ');
  char *names = const_cast<char*>(name.c_str());

  strcpy(names, "");

  vector<double> tract_param;
  tract_param.resize(3 * VocalTract::NUM_PARAMS);

  for (i=0; i < VocalTract::NUM_PARAMS; i++)
  {
    strcat(names, vocalTract->param[i].abbr.c_str());
    if (i != VocalTract::NUM_PARAMS - 1)
    {
      strcat(names, " ");
    }

    tract_param[i] = vocalTract->param[i].min;
    tract_param[i+VocalTract::NUM_PARAMS] = vocalTract->param[i].max;
    tract_param[i+(2*VocalTract::NUM_PARAMS)] = vocalTract->param[i].neutral;
  }
  return tract_param;
}

int VocalTractLab::vtlSetAnatomyParams(vector<double> params)
{
  for (int i=0; i<AnatomyParams::NUM_ANATOMY_PARAMS; i++)
  {
    anatomyParams->param[i].x = params[i];
  }

  anatomyParams->setFor(vocalTract);
  return 0;
}

int VocalTractLab::vtlSetAnatomyParams2(double* params)
{
  for (int i=0; i<AnatomyParams::NUM_ANATOMY_PARAMS; i++)
  {
    anatomyParams->param[i].x = *params;
    params++;
  }

  anatomyParams->setFor(vocalTract);
  return 0;
}

vector<double> VocalTractLab::vtlGetAnatomyParams()
{
  vector<double> ana_params;
  ana_params.resize(13);
  for (int i=0; i<AnatomyParams::NUM_ANATOMY_PARAMS; i++)
  {
    ana_params[i] = anatomyParams->param[i].x;
  }

  return ana_params;
}


int VocalTractLab::vtlSynthesisReset()
{
  synthesizer->reset();
  tube->resetDynamicPart();

  return 0;
}

vector<double> VocalTractLab::vtlSynthAudio(vector<double> tractParams, vector<double> glottisParams, int numFrames, 
  int frameStep_samples)
{
  int i;
  int samplePos = 0;
  int numGlottisParams = (int)glottis[selectedGlottis]->controlParam.size();

  vector<double> audio;
  audio.resize((numFrames-1) * frameStep_samples);

  vtlSynthesisReset();

  for (i = 0; i < numFrames; i++)
  {
    // cout << "In frame #" << i << ":" << endl;
    if (i == 0)
    {
      // Only set the initial state of the vocal tract and glottis without generating audio.
      vector<double> audioVector;
      synthesizer->add(&glottisParams[i*numGlottisParams], 
        &tractParams[i*VocalTract::NUM_PARAMS], 
        0, audioVector);
    }
    else
    {
      vector<double> audioVector;
      synthesizer->add(&glottisParams[i*numGlottisParams], 
        &tractParams[i*VocalTract::NUM_PARAMS], 
        frameStep_samples, audioVector);
      if ((int)audioVector.size() != frameStep_samples)
      {
        throw runtime_error("Error in vtlSynthesisAddTube(): Number of audio samples is wrong.");
        return audio;
      }
      // Copy the audio samples in the given buffer.
      int i;
      for (i = 0; i < frameStep_samples; i++)
      {
        audio[samplePos+i] = audioVector[i];
      }
      samplePos += frameStep_samples;
    }
  }

  return audio;
}

vector<string> VocalTractLab::vtlGetEMANames()
{
  vector<string> ema_names = {"TBX", "TBY", "TMX", "TMY", "TTX", "TTY", "ULX", "ULY", "LLX", "LLY", "JAWX", "JAWY"};

  return ema_names;
}

vector<double> VocalTractLab::vtlTract2EMA(vector<double> tractParams, int numFrames)
{
  vtlSynthesisReset();
  vocalTract->setDefaultEmaPoints();
  int numEmaPoints = (int)vocalTract->emaPoints.size();

  vector<double> ema;
  ema.resize(numFrames * numEmaPoints * 2);
  Point3D P;
  
  for (int i=0; i < numFrames; i++)
  {
    for (int j  = 0; j < VocalTract::NUM_PARAMS; j++)
    {
      VocalTract::Param *param = &(vocalTract->param[j]);
      param->x = tractParams[i*VocalTract::NUM_PARAMS + j];
      param->limitedX = param->x;
    }
    vocalTract->calculateAll();
    for (int k = 0; k < numEmaPoints; k++)
    {
      P = vocalTract->getEmaPointCoord(k);
      ema[i * numEmaPoints * 2 + k * 2] = P.x;
      ema[i * numEmaPoints * 2 + k * 2 + 1] = P.y;
    }
  }

  return ema;
}

int VocalTractLab::vtlExportTractSvg(vector<double> tractParams, const char *fileName, bool addCenterLine, bool addCutVectors)
{

  for (int i=0; i < VocalTract::NUM_PARAMS; i++)
  {
    vocalTract->param[i].x = tractParams[i];
  }

  vocalTract->calculateAll();

  bool ok = vocalTract->exportTractContourSvg(string(fileName), addCenterLine, addCutVectors);
  if (ok)
  {
    return 0;
  }
  else
  {
    return -1;
  }
}

int VocalTractLab::vtlSaveTractFrame( double* tractParams, const char *fileName)
{
  int arg = 0;
  char **argv=NULL;

  glutInit(&arg, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(400, 400); //Set the window size
  glutCreateWindow("Test");

  cout << "Rendering Frame..." << endl;
  for (int i=0; i < VocalTract::NUM_PARAMS; i++)
  {
    vocalTract->param[i].x = *tractParams;
    tractParams++;
  }
  vocalTract->calculateAll();
  vtPicture->setVocalTract(vocalTract);
  vtPicture->display();

  GLint viewport[4];  // current viewport
  glGetIntegerv(GL_VIEWPORT, viewport);  // get current viewport
  int w = viewport[2];
  int h = viewport[3];

  unsigned char* imageData = (unsigned char *)malloc((int)(400*400*3));  // allocate memory for image
  glReadPixels(0, 0, 400, 400, GL_BGR, GL_UNSIGNED_BYTE, imageData);  // copy data
  // build writer
  bitmap_image image(400, 400);
  image_drawer draw(image);

// write from imageData to bitmap image
  for (unsigned int i=0; i < h; i++)
  {
    for (unsigned int j=0; j < w; j++)
    {
      image.set_pixel(j, h-1-i, *(imageData+2), *(imageData+1), *(imageData+3));
      imageData += 3;
    }
  }

  image.save_image(fileName);
  cout << "Rendered Frame ! Saved image to " << fileName << endl;
}

int VocalTractLab::vtlSaveTractVideo(int numFrames, double* tractParams, const char *folderName)
{

  int arg = 0;
  char **argv=NULL;

  glutInit(&arg, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(400, 400); //Set the window size
  glutCreateWindow("Test");

  for (int frameIndex = 0; frameIndex < numFrames; frameIndex++)
  {
    string fileName(folderName);
    fileName += "/vt" + to_string(frameIndex) + ".bmp";

    cout << "Rendering frame #" << frameIndex << "..." << endl;

    for (int i=0; i < VocalTract::NUM_PARAMS; i++)
    {
      vocalTract->param[i].x = *tractParams;
      tractParams++;
    }
    vocalTract->calculateAll();
    vtPicture->setVocalTract(vocalTract);

    vtPicture->display();
    GLint viewport[4];  //current viewport
    glGetIntegerv(GL_VIEWPORT, viewport);  //get current viewport

    int w = viewport[2];
    int h = viewport[3];

    unsigned char* imageData = (unsigned char *)malloc((int)(400*400*(3)));
	  glReadPixels(0, 0, 400, 400, GL_BGR, GL_UNSIGNED_BYTE, imageData);
    //write 
    bitmap_image image(400,400);
    image_drawer draw(image);

    for (unsigned int i = 0; i < h; ++i)
    {
      for (unsigned int j = 0; j < w; ++j)
      {
        image.set_pixel(j, h-1-i, *(imageData+2),*(imageData+1), *(imageData+3));
        imageData += 3;
      }
    }

    image.save_image(fileName);
    cout << "Frame #" << frameIndex << " has stored in " << fileName << endl;
  }
  
}

VocalTractLab* VTL_new(const char *speakerFileName) 
{ 
  return new VocalTractLab(speakerFileName); 
}

int setAnatomy(VocalTractLab* ptr, double *val)
{
  return ptr->vtlSetAnatomyParams2(val);
}

int exportVTFrame(VocalTractLab* ptr, double* tractParams, const char *fileName) 
{ 
  return ptr->vtlSaveTractFrame(tractParams, fileName); 
}

int exportVTVideo(VocalTractLab* ptr, int numFrames, double* tractParams, const char* folderName)
{
  return ptr->vtlSaveTractVideo(numFrames, tractParams, folderName);
}

void VTL_delete(VocalTractLab* ptr)
{
  if (ptr)
  {
    delete ptr;
    ptr = nullptr;
  }
}



