/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                       */
/*    This file is part of the HiGHS linear optimization suite           */
/*                                                                       */
/*    Written and engineered 2008-2021 at the University of Edinburgh    */
/*                                                                       */
/*    Available as open-source under the MIT License                     */
/*                                                                       */
/*    Authors: Julian Hall, Ivet Galabova, Qi Huangfu, Leona Gottwald    */
/*    and Michael Feldmeier                                              */
/*                                                                       */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef HIGHS_DYNAMIC_ROW_MATRIX_H_
#define HIGHS_DYNAMIC_ROW_MATRIX_H_

#include <set>
#include <utility>
#include <vector>

#include "util/HighsInt.h"

class HighsDynamicRowMatrix {
 private:
  /// vector of index ranges in the index and value arrays of AR for each row
  std::vector<std::pair<HighsInt, int>> ARrange_;

  /// column indices for each nonzero in AR
  std::vector<HighsInt> ARindex_;
  /// values for each nonzero in AR
  std::vector<double> ARvalue_;

  std::vector<HighsInt> ARrowindex_;
  std::vector<HighsInt> Anext_;
  std::vector<HighsInt> Aprev_;

  /// vector of pointers to the head/tail of the nonzero block list for each
  /// column
  std::vector<HighsInt> Ahead_;
  std::vector<HighsInt> Atail_;

  /// vector of column sizes

  /// keep an ordered set ofof free spaces in the row arrays so that they can be
  /// reused efficiently
  std::set<std::pair<HighsInt, int>> freespaces_;

  /// vector of deleted rows so that their indices can be reused
  std::vector<HighsInt> deletedrows_;

 public:
  std::vector<HighsInt> Asize_;
  HighsDynamicRowMatrix(HighsInt ncols);

  /// adds a row to the matrix with the given values and returns its index
  HighsInt addRow(HighsInt* Rindex, double* Rvalue, HighsInt Rlen);

  /// removes the row with the given index from the matrix, afterwards the index
  /// can be reused for new rows
  void removeRow(HighsInt rowindex);

  std::size_t nonzeroCapacity() const { return ARvalue_.size(); }

  /// replaces a rows values but does not change the support
  void replaceRowValues(HighsInt rowindex, double* Rvalue);

  /// calls the given function object for each entry in the given column.
  /// The function object should accept the row index as first argument and
  /// the nonzero value of the column in that row as the second argument.
  template <typename Func>
  void forEachColumnEntry(HighsInt col, Func&& f) const {
    HighsInt iter = Ahead_[col];

    while (iter != -1) {
      if (!f(ARrowindex_[iter], ARvalue_[iter])) break;
      iter = Anext_[iter];
    }
  }

  HighsInt getNumRows() const { return ARrange_.size(); }

  HighsInt getNumDelRows() const { return deletedrows_.size(); }

  HighsInt getRowStart(HighsInt row) const { return ARrange_[row].first; }

  HighsInt getRowEnd(HighsInt row) const { return ARrange_[row].second; }

  const HighsInt* getARindex() const { return ARindex_.data(); }

  const double* getARvalue() const { return ARvalue_.data(); }
};

#endif
