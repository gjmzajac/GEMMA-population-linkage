#!/bin/bash

for prefix in eigenlib gemma param io lm lmm bslmm bslmmdap mathfunc mvlmm prdt vc ldr varcov logistic
do
for extension in cpp h
do
cp ${prefix}.${extension} ${prefix}_float.${extension}
# sed -i 's/double/float/g' ${prefix}_float.${extension}
# sed -i 's/_vector_/_vector_float_/g' ${prefix}_float.${extension}
# sed -i '/_vector_int/! s/_vector_/_vector_float_/g' ${prefix}_float.${extension}
perl -pi -e 's/_vector_(?!int)/_vector_float_/g' ${prefix}_float.${extension}
sed -i 's/_vector /_vector_float /g' ${prefix}_float.${extension}
# sed -i '/LogRL_dev/! s/_vector /_vector_float /g' ${prefix}_float.${extension}
# sed -i 's/_matrix_/_matrix_float_/g' ${prefix}_float.${extension}
# sed -i '/_matrix_int/! s/_matrix_/_matrix_float_/g' ${prefix}_float.${extension}
perl -pi -e 's/_matrix_(?!int)/_matrix_float_/g' ${prefix}_float.${extension}
sed -i 's/_matrix /_matrix_float /g' ${prefix}_float.${extension}
# sed -i '/LogRL_dev/! s/_matrix /_matrix_float /g' ${prefix}_float.${extension}
# sed -i 's/ddot/dsdot/g' ${prefix}_float.${extension}
perl -pi -e 's/(?<!lapack_)ddot/dsdot/g' ${prefix}_float.${extension}
sed -i 's/dtrsv/strsv/g' ${prefix}_float.${extension}
sed -i 's/dtrsy/strsy/g' ${prefix}_float.${extension}
sed -i 's/dgemm/sgemm/g' ${prefix}_float.${extension}
sed -i 's/dgemv/sgemv/g' ${prefix}_float.${extension}
sed -i 's/dsyr/ssyr/g' ${prefix}_float.${extension}
sed -i 's/dsyr2/ssyr2/g' ${prefix}_float.${extension}
# sed -i 's/ddot/sdot/g' ${prefix}_float.${extension}
perl -pi -e 's/(?<!lapack_)ddot/dsdot/g' ${prefix}_float.${extension}
sed -i 's/daxpy/saxpy/g' ${prefix}_float.${extension}
sed -i 's/dger/sger/g' ${prefix}_float.${extension}
sed -i 's/Map<Matrix<double/Map<Matrix<float/g' ${prefix}_float.${extension}
sed -i 's/MatrixXd/MatrixXf/g' ${prefix}_float.${extension}
done
done

