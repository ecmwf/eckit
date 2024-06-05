/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <algorithm>
#include <type_traits>

#include "EckitCodecTool.h"

#include "eckit/codec/RecordReader.h"
#include "eckit/codec/codec.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/linalg/SparseMatrix.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/SimpleOption.h"


namespace eckit::tools {


static_assert(std::is_same_v<linalg::Scalar, double>, "Scalar == double");
static_assert(std::is_same_v<linalg::Index, std::int32_t>, "Index == std::int32_t");


class InPlaceAllocator : public linalg::SparseMatrix::Allocator {
public:
    using Layout = linalg::SparseMatrix::Layout;
    using Shape  = linalg::SparseMatrix::Shape;
    using Index  = linalg::Index;
    using Scalar = linalg::Scalar;
    using Size   = linalg::Size;

    InPlaceAllocator(Size Nr, Size Nc, Size nnz, Index* ia, Index* ja, Scalar* a) :
        Nr_(Nr), Nc_(Nc), nnz_(nnz), ia_(ia), ja_(ja), a_(a) {
        ASSERT(ia_ != nullptr);
        ASSERT(ja_ != nullptr);
        ASSERT(a_ != nullptr);
    }

    Layout allocate(Shape& shape) override {
        shape.size_ = nnz_;
        shape.rows_ = Nr_;
        shape.cols_ = Nc_;

        Layout layout;
        layout.outer_ = ia_;
        layout.inner_ = ja_;
        layout.data_  = a_;

        return layout;
    }

    void deallocate(Layout, Shape) override {}

    void print(std::ostream&) const override { NOTIMP; }

    bool inSharedMemory() const override { return false; }

private:
    const Size Nr_;
    const Size Nc_;
    const Size nnz_;
    Index* ia_;  // NOTE: not owned
    Index* ja_;
    Scalar* a_;
};


struct EckitCodecToSparseMatrix : public EckitCodecTool {
    EckitCodecToSparseMatrix(int argc, char** argv) : EckitCodecTool(argc, argv) {
        add_option(new option::SimpleOption<std::string>("nr", "Matrix number of rows"));
        add_option(new option::SimpleOption<std::string>("nc", "Matrix number of columns"));
        add_option(new option::SimpleOption<std::string>("nnz", "CSR matrix number of non-zeros (for checking)"));
        add_option(new option::SimpleOption<std::string>("ia", "CSR matrix row indices key, compressed"));
        add_option(new option::SimpleOption<std::string>("ja", "CSR matrix column indices key"));
        add_option(new option::SimpleOption<std::string>("a", "CSR matrix values key"));
    }

    int numberOfPositionalArguments() override { return 2; }

    std::string usage() override { return name() + " <file.codec> <file.mat> [--help,-h]"; }
    std::string briefDescription() override {
        return "Conversion of specificly formated eckit::codec file into eckit::linalg::SparseMatrix file";
    }
    std::string longDescription() override {
        return briefDescription() +"\n"
                                    "\n       <file.codec>: path to eckit::codec file";
        "\n       <file.mat>: path to eckit::linalg::SparseMatrix file";
    }

    int execute(const Args& args) override {
        ASSERT(args.count() == numberOfPositionalArguments());

        auto Nr_key = args.getString("nr", "nr");
        auto Nc_key = args.getString("nc", "nc");
        auto ia_key = args.getString("ia", "ia");
        auto ja_key = args.getString("ja", "ja");
        auto a_key  = args.getString("a", "a");

        // eckit::codec file
        eckit::PathName fcodec(args(0));
        if (!fcodec.exists()) {
            Log::error() << "File does not exist: " << fcodec << std::endl;
            return failed();
        }

        codec::RecordReader reader(fcodec);

        std::uint64_t version = 0;
        std::uint64_t Nr      = 0;
        std::uint64_t Nc      = 0;
        std::vector<std::int32_t> ia;
        std::vector<std::int32_t> ja;
        std::vector<double> a;

        reader.read("version", version).wait();

        if (version == 0) {
            reader.read(Nr_key, Nr);
            reader.read(Nc_key, Nc);
            reader.read(ia_key, ia);
            reader.read(ja_key, ja);
            reader.read(a_key, a);
            reader.wait();

            if (std::string nnz_key; args.get("nnz", nnz_key)) {
                std::uint64_t nnz = 0;
                reader.read(nnz_key, nnz).wait();

                ASSERT(a.size() == nnz);
            }

            ASSERT(0 < Nr);
            ASSERT(0 < Nc);
            ASSERT(ia.size() == (Nr + 1));
            ASSERT(ja.size() == a.size());
        }
        else {
            throw SeriousBug("unsupported version: " + std::to_string(version), Here());
        }

        // ensure 0-based indexing
        if (const auto base = ia.front(); base != 0) {
            std::for_each(ia.begin(), ia.end(), [base](auto& i) { i -= base; });
        }

        // ensure safe casting
        auto size = [](const auto& v) {
            auto vs = static_cast<linalg::Size>(v);
            ASSERT(v == static_cast<decltype(v)>(vs));
            return vs;
        };

        // create matrix
        linalg::SparseMatrix M(
            new InPlaceAllocator(size(Nr), size(Nc), size(a.size()), const_cast<linalg::Index*>(ia.data()),
                                 const_cast<linalg::Index*>(ja.data()), const_cast<linalg::Scalar*>(a.data())));

        // eckit::linalg::SparseMatrix file
        eckit::PathName fmat(args(1));
        M.save(fmat);

        return 0;
    }
};


}  // namespace eckit::tools


int main(int argc, char** argv) {
    return eckit::tools::EckitCodecToSparseMatrix{argc, argv}.start();
}
