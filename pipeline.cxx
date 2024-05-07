/*
 * Copyright (c) 2021 Triad National Security, LLC, as operator of Los Alamos
 * National Laboratory with the U.S. Department of Energy/National Nuclear
 * Security Administration. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * with the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of TRIAD, Los Alamos National Laboratory, LANL, the
 *    U.S. Government, nor the names of its contributors may be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkCellDataToPointData.h>
#include <vtkCompositePolyDataMapper.h>
#include <vtkContourFilter.h>
#include <vtkDataArraySelection.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPNGWriter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkWindowToImageFilter.h>
#include <vtkXMLMultiBlockDataReader.h>

#include <stdio.h>

void Run(const char *filename) {
  printf("Loading %s ... \n", filename);
  vtkNew<vtkXMLMultiBlockDataReader> reader;
  reader->SetFileName(filename);
  reader->UpdateInformation();
  reader->GetCellDataArraySelection()->DisableAllArrays();
  reader->GetCellDataArraySelection()->EnableArray("v02");
  vtkNew<vtkCellDataToPointData> f1;
  f1->SetInputConnection(reader->GetOutputPort());
  f1->UpdateInformation();
  f1->ProcessAllArraysOff();
  f1->AddCellDataArray("v02");
  vtkNew<vtkContourFilter> f2;
  f2->SetInputConnection(f1->GetOutputPort());
  f2->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS,
                             "v02");
  f2->SetNumberOfContours(10);
  f2->SetValue(0, 1e-6);
  f2->SetValue(1, 4.641588833612782e-06);
  f2->SetValue(2, 2.1544346900318823e-05);
  f2->SetValue(3, 0.0001);
  f2->SetValue(4, 0.00046415888336127773);
  f2->SetValue(5, 0.002154434690031882);
  f2->SetValue(6, 0.01);
  f2->SetValue(7, 0.046415888336127725);
  f2->SetValue(8, 0.21544346900318823);
  f2->SetValue(9, 1);

  vtkNew<vtkCompositePolyDataMapper> mapper;
  mapper->SetInputConnection(f2->GetOutputPort());
  // mapper->ScalarVisibilityOff();

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  vtkNew<vtkNamedColors> colors;
  actor->GetProperty()->SetColor(colors->GetColor3d("Peacock").GetData());
  actor->GetProperty()->EdgeVisibilityOn();

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("Beige").GetData());
  renderer->ResetCamera();

  vtkNew<vtkRenderWindow> win;
  win->AddRenderer(renderer);
  win->SetSize(1024, 764);
  win->SetWindowName("XX");
  win->Render();

  vtkNew<vtkWindowToImageFilter> image;
  image->SetInput(win);
  image->Update();

  vtkNew<vtkPNGWriter> png;
  png->SetFileName("output.png");
  png->SetInputConnection(image->GetOutputPort());
  png->Write();
}

int main(int argc, char *argv[]) {
  Run(argv[1]);
  return 0;
}
