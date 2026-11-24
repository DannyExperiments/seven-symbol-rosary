# Building the paper

```sh
cd paper && latexmk -pdf manuscript.tex
```

Built with TeX Live (`amsart`, `amsplain`); the CI workflow `pdf.yml` rebuilds the PDF from source on every push. The PDF was inspected page by page after building.
