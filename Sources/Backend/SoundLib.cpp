// ****************************************************************************
// This file is part of VocalTractLab.
// Copyright (C) 2020, Peter Birkholz, Dresden, Germany
// Copyright (C) 2016, Thomas Uhle, Dresden, Germany
// www.vocaltractlab.de
// author: Peter Birkholz
// author: Thomas Uhle (OpenAL implementation)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// ****************************************************************************

#include "SoundLib.h"
#include <cstdio>
#include <memory>

// ----------------------------------------------------------------------------
// Audio library for systems with OpenAL.
// ----------------------------------------------------------------------------
// #ifdef HAVE_OPENAL

#if defined(WIN32)
#include <al.h>
#include <alc.h>
#elif defined(__APPLE__)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#error "Check your operation system !"
#include <AL/al.h>
#include <AL/alc.h>
#endif

namespace
{
  class SoundOpenAL : public SoundInterface
  {
  public:
    SoundOpenAL();
    virtual ~SoundOpenAL();
    virtual bool init(int samplingRate);
    virtual bool startPlayingWave(signed short *data, int numSamples, bool loop);
    virtual bool stopPlaying();
    virtual bool startRecordingWave(signed short *data, int numSamples);
    virtual bool stopRecording();

  private:
    ALCcontext *ctx;
    ALCdevice *devCapture;
    ALCvoid *dataCapture;
    ALuint buf;
    ALuint src;
    ALsizei samplingRate;
    bool hasCaptureExt;
    bool isPlaying;
    bool isRecording;
    bool isInitialized;
  };
}


// ****************************************************************************
// ****************************************************************************

SoundOpenAL::SoundOpenAL() :
  ctx(NULL),
  devCapture(NULL),
  dataCapture(NULL),
  buf(0),
  src(0),
  samplingRate(44100),   // Sampling rate; it will be overwritten in init().
  hasCaptureExt(false),
  isPlaying(false),
  isRecording(false),
  isInitialized(false)
{
}

// ****************************************************************************
// ****************************************************************************

bool SoundOpenAL::init(int samplingRate)
{
  ALCdevice *dev = NULL;

  if (isInitialized)
  {
    return false;
  }

  this->samplingRate = samplingRate;

  dev = alcOpenDevice(NULL);
  if (dev == NULL)
  {
    fprintf(stderr, "Unable to open OpenAL device.\n");
    return false;
  }

  if (alcIsExtensionPresent(dev, "ALC_EXT_CAPTURE") == AL_TRUE)
  {
    hasCaptureExt = true;
  }
  else
  {
    hasCaptureExt = false;
    printf("This OpenAL device cannot capture sound.\n");
  }

  ctx = alcCreateContext(dev, NULL);
  if (ctx == NULL)
  {
    fprintf(stderr, "Failed to create OpenAL context.\n");
    alcCloseDevice(dev);
    return false;
  }

  if (!alcMakeContextCurrent(ctx))
  {
    fprintf(stderr, "Failed to make default context.\n");
    alcDestroyContext(ctx);
    alcCloseDevice(dev);
    return false;
  }

  alGenBuffers(1, &buf);
  if (alGetError() != AL_NO_ERROR)
  {
    fprintf(stderr, "Failed to create sound buffer.\n");
    return false;
  }

  alGenSources(1, &src);
  if (alGetError() != AL_NO_ERROR)
  {
    fprintf(stderr, "Failed to create sound source.\n");
    return false;
  }

  isInitialized = true;

  return true;
}

// ****************************************************************************
// ****************************************************************************

SoundOpenAL::~SoundOpenAL()
{
  ALCdevice *dev = NULL;

  if (isInitialized)
  {
    stopPlaying();
    stopRecording();
    alDeleteBuffers(1, &buf);
    alDeleteSources(1, &src);
    dev = alcGetContextsDevice(ctx);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(ctx);
    alcCloseDevice(dev);
  }
}

// ****************************************************************************
// Starts playback in an infinite loop.
// ****************************************************************************

bool SoundOpenAL::startPlayingWave(signed short *data, int numSamples, bool loop)
{
  ALsizei dataSize = numSamples * 2;

  if (!isInitialized)
  {
    return false;
  }

  if (!stopPlaying())
  {
    return false;
  }

  alBufferData(buf, AL_FORMAT_MONO16, data, dataSize, samplingRate);
  if (alGetError() != AL_NO_ERROR)
  {
    fprintf(stderr, "Failed to load sound buffer.\n");
    return false;
  }

  alSourceQueueBuffers(src, 1, &buf);
  if (alGetError() != AL_NO_ERROR)
  {
    fprintf(stderr, "Failed to queue sound buffer for playing.\n");
    return false;
  }

  alSourcei(src, AL_LOOPING, (loop)? AL_TRUE : AL_FALSE);
  if (alGetError() != AL_NO_ERROR)
  {
    fprintf(stderr, "Failed to set looping mode.\n");
    return false;
  }

  alSourcePlay(src);
  if (alGetError() != AL_NO_ERROR)
  {
    fprintf(stderr, "Failed to start playing sound.\n");
    return false;
  }

  isPlaying = true;

  return true;
}

// ****************************************************************************
// ****************************************************************************

bool SoundOpenAL::stopPlaying()
{
  if (!isInitialized)
  {
    return false;
  }

  if (isPlaying)
  {
    ALuint tmpBuf;

    alSourceStop(src);

    alSourcei(src, AL_LOOPING, AL_FALSE);
    if (alGetError() != AL_NO_ERROR)
    {
      fprintf(stderr, "Failed to unset looping mode.\n");
      return false;
    }

    alSourceUnqueueBuffers(src, 1, &tmpBuf);
    if (alGetError() != AL_NO_ERROR)
    {
      fprintf(stderr, "Failed to remove sound buffer from queue.\n");
      return false;
    }

    isPlaying = false;
  }

  return true;
}

// ****************************************************************************
// Starts recording into a ring buffer.
// ****************************************************************************

bool SoundOpenAL::startRecordingWave(signed short *data, int numSamples)
{
  ALsizei dataSize = numSamples * 2;

  if (!(hasCaptureExt && isInitialized))
  {
    return false;
  }

  if (!stopRecording())
  {
    return false;
  }

  devCapture = alcCaptureOpenDevice(NULL, samplingRate, AL_FORMAT_MONO16, dataSize);
  if (alGetError() != AL_NO_ERROR)
  {
    fprintf(stderr, "Unable to open OpenAL device for sound capture.\n");
    return false;
  }

  alcCaptureStart(devCapture);
  if (alGetError() != AL_NO_ERROR)
  {
    fprintf(stderr, "Failed to start sound capture.\n");
    return false;
  }

  dataCapture = data;
  isRecording = true;

  return true;
}

// ****************************************************************************
// ****************************************************************************

bool SoundOpenAL::stopRecording()
{
  if (!(hasCaptureExt && isInitialized))
  {
    return false;
  }

  if (isRecording)
  {
    ALCint numSamples = 0;

    alcGetIntegerv(devCapture, ALC_CAPTURE_SAMPLES, 1, &numSamples);
    if (alGetError() != AL_NO_ERROR)
    {
      fprintf(stderr, "Failed to get number of captured samples.\n");
      return false;
    }

    alcCaptureSamples(devCapture, dataCapture, numSamples);
    if (alGetError() != AL_NO_ERROR)
    {
      fprintf(stderr, "Failed to copy sound samples.\n");
      return false;
    }

    alcCaptureStop(devCapture);
    if (alGetError() != AL_NO_ERROR)
    {
      fprintf(stderr, "Failed to stop sound capture.\n");
      return false;
    }

    if (alcCaptureCloseDevice(devCapture) == AL_FALSE)
    {
      fprintf(stderr, "Unable to close OpenAL device for sound capture.\n");
      return false;
    }

    dataCapture = NULL;
    isRecording = false;
  }

  return true;
}

// #endif


// ----------------------------------------------------------------------------
// Implementation of generic sound interface (Singleton pattern).
// ----------------------------------------------------------------------------
SoundInterface *SoundInterface::getInstance()
{
#if defined(__APPLE__)
  static std::auto_ptr<SoundInterface> instance(new SoundOpenAL);
#elif defined(WIN32)
  static std::auto_ptr<SoundInterface> instance(new SoundWinMM);
#else
# error "Windows or OpenAL please !"
#endif

  return instance.get();
}

