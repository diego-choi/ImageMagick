/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                         W   W   AAA   N   N  DDDD                           %
%                         W   W  A   A  NN  N  D   D                          %
%                         W W W  AAAAA  N N N  D   D                          %
%                         WW WW  A   A  N  NN  D   D                          %
%                         W   W  A   A  N   N  DDDD                           %
%                                                                             %
%                                                                             %
%                         MagickWand Support Methods                          %
%                                                                             %
%                              Software Design                                %
%                                   Cristy                                    %
%                                  May 2004                                   %
%                                                                             %
%                                                                             %
%  Copyright @ 1999 ImageMagick Studio LLC, a non-profit organization         %
%  dedicated to making software imaging solutions freely available.           %
%                                                                             %
%  You may not use this file except in compliance with the License.  You may  %
%  obtain a copy of the License at                                            %
%                                                                             %
%    https://imagemagick.org/script/license.php                               %
%                                                                             %
%  Unless required by applicable law or agreed to in writing, software        %
%  distributed under the License is distributed on an "AS IS" BASIS,          %
%  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   %
%  See the License for the specific language governing permissions and        %
%  limitations under the License.                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Assign unique ID's to each magick wand.  We store the ID's in a splay-tree
% to track which ID's persist and which were destroyed.
%
%
*/

/*
  Include declarations.
*/
#include "MagickWand/studio.h"
#include "MagickWand/MagickWand.h"
#include "MagickWand/magick-wand-private.h"
#include "MagickWand/wand.h"

static SplayTreeInfo
  *wand_ids = (SplayTreeInfo *) NULL;

static MagickBooleanType
  instantiate_wand = MagickFalse;

static SemaphoreInfo
  *wand_semaphore = (SemaphoreInfo *) NULL;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A c q u i r e W a n d I d                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AcquireWandId() returns a unique wand id.
%
%  The format of the AcquireWandId() method is:
%
%      size_t AcquireWandId()
%
*/
WandExport size_t AcquireWandId(void)
{
  const size_t
    *wand_id = (const size_t *) NULL;

  size_t
    id;

  static size_t
    isn = 0;

  if (wand_semaphore == (SemaphoreInfo *) NULL)
    ActivateSemaphoreInfo(&wand_semaphore);
  LockSemaphoreInfo(wand_semaphore);
  if (wand_ids == (SplayTreeInfo *) NULL)
    wand_ids=NewSplayTree((int (*)(const void *,const void *)) NULL,
      (void *(*)(void *)) NULL,(void *(*)(void *)) NULL);
  id=isn++;
  (void) AddValueToSplayTree(wand_ids,wand_id+id,wand_id+id);
  instantiate_wand=MagickTrue;
  UnlockSemaphoreInfo(wand_semaphore);
  return(id);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%    D e s t r o y W a n d I d s                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyWandIds() deallocates memory associated with the wand id's.
%
%  The format of the DestroyWandIds() method is:
%
%      void DestroyWandIds(void)
%
%  A description of each parameter follows:
%
*/
WandExport void DestroyWandIds(void)
{
  if (wand_semaphore == (SemaphoreInfo *) NULL)
    ActivateSemaphoreInfo(&wand_semaphore);
  LockSemaphoreInfo(wand_semaphore);
  if (wand_ids != (SplayTreeInfo *) NULL)
    wand_ids=DestroySplayTree(wand_ids);
  instantiate_wand=MagickFalse;
  UnlockSemaphoreInfo(wand_semaphore);
  RelinquishSemaphoreInfo(&wand_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%    R e l i n q u i s h W a n d I d                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  RelinquishWandId() relinquishes a unique wand id.
%
%  The format of the RelinquishWandId() method is:
%
%      void RelinquishWandId(const size_t *id)
%
%  A description of each parameter follows:
%
%    o id: a unique wand id.
%
*/
WandExport void RelinquishWandId(const size_t id)
{
  const size_t
    *wand_id = (const size_t *) NULL;

  LockSemaphoreInfo(wand_semaphore);
  if (wand_ids != (SplayTreeInfo *) NULL)
    (void) DeleteNodeFromSplayTree(wand_ids,wand_id+id);
  UnlockSemaphoreInfo(wand_semaphore);
}
