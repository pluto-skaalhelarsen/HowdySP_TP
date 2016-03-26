/* ---------------------------------------------------------------------
 * Numenta Platform for Intelligent Computing (NuPIC)
 * Copyright (C) 2013-2015, Numenta, Inc.  Unless you have an agreement
 * with Numenta, Inc., for a separate license for this software code, the
 * following terms and conditions apply:
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Affero Public License for more details.
 *
 * You should have received a copy of the GNU Affero Public License
 * along with this program.  If not, see http://www.gnu.org/licenses.
 *
 * http://numenta.org/licenses/
 * ---------------------------------------------------------------------
 */

#include <iostream>
#include <vector>
#include <algorithm>    // std::generate
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand
#include <cmath> 	// pow

#include "CmdLineParse.hpp"
#include "BufferSummary.hpp"

#include "nupic/algorithms/SpatialPooler.hpp"
#include "nupic/algorithms/Cells4.hpp"
#include "nupic/os/Timer.hpp"

using namespace std;
using namespace nupic;
using nupic::algorithms::spatial_pooler::SpatialPooler;
using nupic::algorithms::Cells4::Cells4;

	// function generator:
//int RandomNumber01 () { return (rand()%2); } // returns random (binary) numbers from {0,1}

	// Genreates about 200 ones in a 10k buffer and returns random (binary) numbers from {0,1}
int RandomNumber01 ()
{
  int ret = rand();
  if(ret < 85899346)
    return (rand() & 1);
  else
    return(0);
}


int main(int argc, const char * argv[])
{
const UInt DIM = 2048; // number of columns in SP, TP
const UInt DIM_INPUT = 10000;
const UInt TP_CELLS_PER_COL = 10; // cells per column in TP
const UInt EPOCHS = pow(10, 4); // number of iterations (calls to SP/TP compute() )

  UInt Epochs = EPOCHS;

  CmdLineParse *CmdLine = new CmdLineParse(argc, argv);
  srand(CmdLine->Vars("RngSeed"));	// default is zero
  if(CmdLine->Vars("Epochs"))
    Epochs = CmdLine->Vars("Epochs");

  vector<UInt> inputDim = {DIM_INPUT};
  vector<UInt> colDim = {DIM};

  // generate random input
  vector<UInt> input(DIM_INPUT);
  vector<UInt> outSP(DIM); // active array, output of SP/TP
  const int _CELLS = DIM * TP_CELLS_PER_COL;
  vector<UInt> outTP(_CELLS);   
  Real rIn[DIM] = {}; // input for TP (must be Reals)
  Real rOut[_CELLS] = {};

	// ***** Initialize the spatial pooler using the given parameters. *****
  SpatialPooler sp(
/*              inputDimensions A list of integers representing the
                dimensions of the input vector. Format is [height, width,
                depth, ...], where each value represents the size of the
                dimension. For a topology of one dimesion with 100 inputs
                use [100]. For a two dimensional topology of 10x5
                use [10,5].

 */
        inputDim,
/*              columnDimensions A list of integers representing the
                dimensions of the columns in the region. Format is [height,
                width, depth, ...], where each value represents the size of
                the dimension. For a topology of one dimesion with 2000
                columns use 2000, or [2000]. For a three dimensional
                topology of 32x64x16 use [32, 64, 16].

 */
        colDim,
/*              potentialRadius This parameter deteremines the extent of the
                input that each column can potentially be connected to. This
                can be thought of as the input bits that are visible to each
                column, or a 'receptive field' of the field of vision. A large
                enough value will result in global coverage, meaning
                that each column can potentially be connected to every input
                bit. This parameter defines a square (or hyper square) area: a
                column will have a max square potential pool with sides of
                length (2 * potentialRadius + 1).

 */
        16,
/*              potentialPct The percent of the inputs, within a column's
                potential radius, that a column can be connected to. If set to
                1, the column will be connected to every input within its
                potential radius. This parameter is used to give each column a
                unique potential pool when a large potentialRadius causes
                overlap between the columns. At initialization time we choose
                ((2*potentialRadius + 1)^(# inputDimensions) * potentialPct)
                input bits to comprise the column's potential pool.

 */
        0.5,
/*              globalInhibition If true, then during inhibition phase the
                winning columns are selected as the most active columns from the
                region as a whole. Otherwise, the winning columns are selected
                with resepct to their local neighborhoods. Global inhibition
                boosts performance significantly but there is no topology at the
                output.

 */
        true,
/*              localAreaDensity The desired density of active columns within
                a local inhibition area (the size of which is set by the
                internally calculated inhibitionRadius, which is in turn
                determined from the average size of the connected potential
                pools of all columns). The inhibition logic will insure that at
                most N columns remain ON within a local inhibition area, where
                N = localAreaDensity * (total number of columns in inhibition
                area). If localAreaDensity is set to a negative value output
                sparsity will be determined by the numActivePerInhArea.

 */
        -1.0,
/*              numActiveColumnsPerInhArea An alternate way to control the sparsity of
                active columns. If numActivePerInhArea is specified then
                localAreaDensity must be less than 0, and vice versa. When
                numActivePerInhArea > 0, the inhibition logic will insure that
                at most 'numActivePerInhArea' columns remain ON within a local
                inhibition area (the size of which is set by the internally
                calculated inhibitionRadius). When using this method, as columns
                learn and grow their effective receptive fields, the
                inhibitionRadius will grow, and hence the net density of the
                active columns will *decrease*. This is in contrast to the
                localAreaDensity method, which keeps the density of active
                columns the same regardless of the size of their receptive
                fields.

 */
        10,
/*              stimulusThreshold This is a number specifying the minimum
                number of synapses that must be active in order for a column to
                turn ON. The purpose of this is to prevent noisy input from
                activating columns.

 */
        0,
/*              synPermInactiveDec The amount by which the permanence of an
                inactive synapse is decremented in each learning step.

 */
        0.008,
/*              synPermActiveInc The amount by which the permanence of an
                active synapse is incremented in each round.

 */
        0.05,
/*              synPermConnected The default connected threshold. Any synapse
                whose permanence value is above the connected threshold is
                a "connected synapse", meaning it can contribute to
                the cell's firing.

 */
        0.1,
/*              minPctOverlapDutyCycles A number between 0 and 1.0, used to set
                a floor on how often a column should have at least
                stimulusThreshold active inputs. Periodically, each column looks
                at the overlap duty cycle of all other column within its
                inhibition radius and sets its own internal minimal acceptable
                duty cycle to: minPctDutyCycleBeforeInh * max(other columns'
                duty cycles). On each iteration, any column whose overlap duty
                cycle falls below this computed value will get all of its
                permanence values boosted up by synPermActiveInc. Raising all
                permanences in response to a sub-par duty cycle before
                inhibition allows a cell to search for new inputs when either
                its previously learned inputs are no longer ever active, or when
                the vast majority of them have been "hijacked" by other columns.

 */
        0.001,
/*              minPctActiveDutyCycles A number between 0 and 1.0, used to set
                a floor on how often a column should be activate. Periodically,
                each column looks at the activity duty cycle of all other
                columns within its inhibition radius and sets its own internal
                minimal acceptable duty cycle to:

                    minPctDutyCycleAfterInh * max(other columns' duty cycles).

                On each iteration, any column whose duty cycle after inhibition
                falls below this computed value will get its internal boost
                factor increased.

 */
        0.001,
/*              dutyCyclePeriod The period used to calculate duty cycles.
                Higher values make it take longer to respond to changes in
                boost. Shorter values make it potentially more unstable and
                likely to oscillate.

 */
        1000,
/*              maxBoost The maximum overlap boost factor. Each column's
                overlap gets multiplied by a boost factor before it gets
                considered for inhibition. The actual boost factor for a column
                is a number between 1.0 and maxBoost. A boost factor of 1.0 is
                used if the duty cycle is >= minOverlapDutyCycle, maxBoost is
                used if the duty cycle is 0, and any duty cycle in between is
                linearly extrapolated from these 2 endpoints.

 */
        10.0,
/*              seed Seed for our random number generator. If seed is < 0
                a randomly generated seed is used. The behavior of the spatial
                pooler is deterministic once the seed is set.

 */
        1,
/*              spVerbosity spVerbosity level: 0, 1, 2, or 3

 */
        CmdLine->Vars("SpVerbosity"),
/*              wrapAround boolean value that determines whether or not inputs
                at the beginning and end of an input dimension are considered
                neighbors for the purpose of mapping inputs to columns.
 */
        true);


	// ***** Initialize the temporal pooler using the given parameters. *****
  Cells4 tp(
        DIM,			// UInt nColumns =0
        TP_CELLS_PER_COL,	// UInt nCellsPerCol =0
        12,			// UInt activationThreshold =1
        8,			// UInt minThreshold =1
        15,			// UInt newSynapseCount =1
        5,			// UInt segUpdateValidDuration =1
        0.5,			// Real permInitial =.5
        0.8,			// Real permConnected =.8
        1.0,			// Real permMax =1
        1,			// Real permDec =.1
        1,			// Real permInc =.1
        0.0,			// Real globalDecay =0
        false,			// bool doPooling =false
        42,			// int seed =-1
        true,			// bool initFromCpp =false
        false);			// bool checkSynapseConsistency =false

	// Set the TP verbosity level
  tp.setVerbosity(CmdLine->Vars("TpVerbosity"));

	// Start a stopwatch timer
  Timer stopwatch(true);

	// ***** Run the learning sequence *****
  for (UInt e = 0; e < Epochs; e++) {
    if(CmdLine->Flags("--alternate_reverse"))
    {
      if(e & 1)
        reverse(outSP.begin(), outSP.end());
      else
        generate(input.begin(), input.end(), RandomNumber01);
    }
    else
        generate(input.begin(), input.end(), RandomNumber01);

    fill(outSP.begin(), outSP.end(), 0);
    sp.compute(input.data(), true, outSP.data());
    sp.stripUnlearnedColumns(outSP.data());

    for (UInt i = 0; i < DIM; i++) {
      rIn[i] = (Real)(outSP[i]);
    }

    tp.compute(rIn, rOut, true, true);

    for (UInt i=0; i< _CELLS; i++) {
      outTP[i] = (UInt)rOut[i];
    }

    if (e == Epochs-1)
    {
	// print entire final arrays
      if(CmdLine->Flags("--dump_final"))
      {
        cout << "Epoch = " << e << endl;
        cout << "SP=" << outSP << endl;
        cout << "TP=" << outTP << endl;
      }
	// print a list of non-zero runs
      if(CmdLine->Flags("--sp_summary"))
      {
        BufferSummary *SpSummary = new BufferSummary(outSP);
        cout << "Epoch " << e << endl;
        cout << "Non-zero elements of final SP output (location : length) " << endl;
        while(SpSummary->PrepareNextRunMsg())
          cout << SpSummary->Msg << endl;
      }

      if(CmdLine->Flags("--tp_summary"))
      {
        BufferSummary *TpSummary = new BufferSummary(outTP);
        cout << "Epoch " << e << endl;
        cout << "Non-zero elements of final TP output (location : length) " << endl;
        while(TpSummary->PrepareNextRunMsg())
          cout << TpSummary->Msg << endl;
      }

    }

  }


  stopwatch.stop();
  cout << "Total elapsed time = " << stopwatch.getElapsed() << " seconds" << endl;

  return 0;
}
